#include "simple_crawler.h"
#include "url_extractor.h"

#include <third_party/g3log/src/g2log.hpp>

#include <library/crawler/simple_crawler_impl/coordinator.h>
#include <library/crawler/simple_crawler_impl/graph.h>
#include <library/crawler/simple_crawler_impl/queue.h>
#include <library/crawler/simple_crawler_impl/url_to_id.h>
#include <library/crawler/simple_crawler_impl/urls.h>
#include <library/crawler/simple_crawler_impl/vector.h>
#include <library/download/wget.h>
#include <library/graph/graph.h>
#include <library/protobuf_helpers/serialization.h>
#include <library/save_load/save_load.h>

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <cstdint>


class ycrawler::SimpleCrawler::Impl {
public:
    void SetConfig(const SimpleCrawlerConfig& config) {
        config_ = config;
        Init();
    }

    void Start() {
        should_save_ = true;
        StartImpl();
        Save();
        should_save_ = false;
    }

    void Stop() {
        throw std::runtime_error{"Unimplemented"};
    }

    void Restore(const std::string& config_file_name) {
        yproto::ReadDelimitedFromFile(config_file_name, config_);
        Load();
    }

    void MoveFailedToQueue() {
        urls_queue_.PushMany(failed_urls_.Get());
    }

    ~Impl() noexcept {
        if (should_save_) {
            if (valid_) {
                Save();
            }
        }
    }

private:
    void ProcessUrl();

    void StartImpl();

    void Init() {
        LOG(INFO) << "Crawler initialization started";
        const auto url_statuses = url_to_id_.Add(
            config_.urls_seed().begin(), config_.urls_seed().end()
        );
        for (const auto& url_status: url_statuses) {
            urls_queue_.Push({url_status.id, 0});
        }
        config_.clear_urls_seed();
        valid_ = true;
        LOG(INFO) << "Crawler initialization finished";
    }

    void Load() {
        LOG(INFO) << "Crawler state loading started";
        const auto& state_dir = config_.state().directory();
        urls_queue_.Load(state_dir + config_.state().queued_urls_file_name());
        failed_urls_.Set(ysave_load::Load<std::vector<sci::url::UrlId>>(
            state_dir + config_.state().failed_urls_file_name()
        ));
        processed_urls_.Set(ysave_load::Load<std::vector<sci::url::UrlId>>(
            state_dir + config_.state().processed_urls_file_name()
        ));
        const auto& doc_data_dir = config_.documents().documents_data_directory();
        url_to_id_.Load(doc_data_dir + config_.documents().url_to_id_file_name());
        web_graph_.Set(ysave_load::Load<ygraph::SparceGraph<sci::url::UrlId>>(
            doc_data_dir + config_.documents().web_graph_file_name()
        ));
        valid_ = true;
        should_save_ = true;
        LOG(INFO) << "Crawler state loading ended";
    }

    void Save() noexcept {
        LOG(INFO) << "Crawler state saving started";
        const auto& state_dir = config_.state().directory();
        try {
            urls_queue_.Save(state_dir + config_.state().queued_urls_file_name());
        } catch (const std::exception& exc) {
            LOGF(WARNING, "exception occured: %s", exc.what());
        }

        try {
            ysave_load::Save(
                failed_urls_.Get(), state_dir + config_.state().failed_urls_file_name()
            );
        } catch (const std::exception& exc) {
            LOGF(WARNING, "exception occured: %s", exc.what());
        }

        try {
            ysave_load::Save(
                processed_urls_.Get(), state_dir + config_.state().processed_urls_file_name()
            );
        } catch (const std::exception& exc) {
            LOGF(WARNING, "exception occured: %s", exc.what());
        }

        const auto& doc_data_dir = config_.documents().documents_data_directory();
        try {
            url_to_id_.Save(doc_data_dir + config_.documents().url_to_id_file_name());
        } catch (const std::exception& exc) {
            LOGF(WARNING, "exception occured: %s", exc.what());
        }

        try {
            ysave_load::Save(
                web_graph_.Get(),
                doc_data_dir + config_.documents().web_graph_file_name()
            );
        } catch (const std::exception& exc) {
            LOGF(WARNING, "exception occured: %s", exc.what());
        }

        // We write config as the last one
        try {
            yproto::WriteDelimitedToFile(config_, state_dir + config_.state().config_file_name());
        } catch (const std::exception& exc) {
            LOGF(WARNING, "exception occured: %s", exc.what());
        }
        LOG(INFO) << "Crawler state saving finished";
    }


    SimpleCrawlerConfig config_;

    bool should_save_ = false;
    bool valid_ = false;

    sci::UrlToId url_to_id_;
    sci::UrlsQueue urls_queue_;
    sci::VectorWithMutex<sci::url::UrlId> failed_urls_;
    sci::VectorWithMutex<sci::url::UrlId> processed_urls_;

    sci::SparceGraphWithMutex<sci::url::UrlId> web_graph_;

    sci::Coordinator coordinator_;
};


// ycrawler::SimpleCrawler implementation BEGIN

void ycrawler::SimpleCrawler::SetConfig(const SimpleCrawlerConfig& config) {
    pimpl_->SetConfig(config);
}


void ycrawler::SimpleCrawler::Start() {
    pimpl_->Start();
}


void ycrawler::SimpleCrawler::Stop() {
    pimpl_->Stop();
}


void ycrawler::SimpleCrawler::Restore(const std::string& config_file_name) {
    pimpl_->Restore(config_file_name);
}


void ycrawler::SimpleCrawler::MoveFailedToQueue() {
    pimpl_->MoveFailedToQueue();
}


// We implement constructor inside cpp file because of PImpl patter we use
ycrawler::SimpleCrawler::SimpleCrawler()
    : pimpl_{std::make_unique<Impl>()} {
}


// We implement destructor inside cpp file because of PImpl pattern we use
ycrawler::SimpleCrawler::~SimpleCrawler() {
}

// ycrawler::SimpleCrawler implementation END


static std::string ReadFile(const std::string& file_name) {
    std::ifstream file(file_name);
    return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
}


void ycrawler::SimpleCrawler::Impl::ProcessUrl() {
    const auto downloader = std::make_unique<ydownload::WgetDownloader>();
    const auto link_extractor = std::make_unique<ycrawler::SimpleWikipediaUrlExtractor>();
    const auto queue_response = urls_queue_.Pop();
    LOGF(DEBUG, "Queue size %zu", queue_response.size);
    if (0 == queue_response.size) {
        return;
    }
    const auto url = url_to_id_.Get(queue_response.url_with_tries.id);
    LOGF(INFO, "Processing url: %s", url.c_str());
    const auto url_file_name = config_.documents().documents_directory()
                                + std::to_string(queue_response.url_with_tries.id);
    const auto response = downloader->Download(url, url_file_name);
    if (!response.Success) {
        if (queue_response.url_with_tries.tries == config_.tries_limit()) {
            failed_urls_.Push(queue_response.url_with_tries.id);
        } else {
            urls_queue_.Push({
                queue_response.url_with_tries.id, queue_response.url_with_tries.tries + 1
            });
        }
        return;
    }

    const auto url_content = ReadFile(url_file_name);
    const auto links = link_extractor->Extract(url_content);
    LOGF(DEBUG, "Links found:  %zu", links.size());
    const auto link_statuses = url_to_id_.Add(links.begin(), links.end());
    for (const auto& link_status: link_statuses) {
        if (!link_status.known) {
            urls_queue_.Push({link_status.id, 0});
        }
    }

    auto out_url_ids = std::vector<sci::url::UrlId>(link_statuses.size());
    for (auto index = size_t{}; index < link_statuses.size(); ++index) {
        out_url_ids[index] = link_statuses[index].id;
    }
    web_graph_.Add(queue_response.url_with_tries.id, std::move(out_url_ids));
    processed_urls_.Push(queue_response.url_with_tries.id);
}


void ycrawler::SimpleCrawler::Impl::StartImpl() {
    auto stop_condition_checker = std::function<bool()>{};
    if (config_.processed_urls_is_limited()) {
        stop_condition_checker = [this]{
            return this->processed_urls_.Size() >= this->config_.processed_urls_limit();
        };
    } else {
        stop_condition_checker = [this]{ return this->urls_queue_.Empty(); };
    }

    coordinator_.Run(
        [this]{ this->ProcessUrl(); },
        config_.threads(),
        stop_condition_checker,
        [this]{ this->Save(); },
        config_.backup_frequency()
    );
}

