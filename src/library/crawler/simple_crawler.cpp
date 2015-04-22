#include "simple_crawler.h"
#include "url_extractor.h"

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

#include <iostream>
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
        const auto url_statuses = url_to_id_.Add(
            config_.urls_seed().begin(), config_.urls_seed().end()
        );
        for (const auto& url_status: url_statuses) {
            urls_queue_.Push({url_status.id, 0});
        }
        config_.clear_urls_seed();
        valid_ = true;
    }

    void Load() {
        const auto& dir = config_.state().directory();
        url_to_id_.Load(dir + config_.state().url_to_id_file_name());
        urls_queue_.Load(dir + config_.state().queued_urls_file_name());
        failed_urls_.Set(ysave_load::Load<std::vector<sci::url::UrlId>>(
            dir + config_.state().failed_urls_file_name()
        ));
        processed_urls_.Set(ysave_load::Load<std::vector<sci::url::UrlId>>(
            dir + config_.state().processed_urls_file_name()
        ));
        web_graph_.Set(ysave_load::Load<ygraph::SparceGraph<sci::url::UrlId>>(
            config_.documents().documents_data_directory()
            + config_.documents().web_graph_file_name()
        ));
        valid_ = true;
        should_save_ = true;
    }

    void Save() noexcept {
        const auto& dir = config_.state().directory();
        try {
            url_to_id_.Save(dir + config_.state().url_to_id_file_name());
        } catch (const std::exception& exc) {
            std::cerr << __FILE__ << ':' << __LINE__ << " EXCEPTION: " << exc.what() << std::endl;
        }

        try {
            urls_queue_.Save(dir + config_.state().queued_urls_file_name());
        } catch (const std::exception& exc) {
            std::cerr << __FILE__ << ':' << __LINE__ << " EXCEPTION: " << exc.what() << std::endl;
        }

        try {
            ysave_load::Save(failed_urls_.Get(), dir + config_.state().failed_urls_file_name());
        } catch (const std::exception& exc) {
            std::cerr << __FILE__ << ':' << __LINE__ << " EXCEPTION: " << exc.what() << std::endl;
        }

        try {
            ysave_load::Save(
                processed_urls_.Get(), dir + config_.state().processed_urls_file_name()
            );
        } catch (const std::exception& exc) {
            std::cerr << __FILE__ << ':' << __LINE__ << " EXCEPTION: " << exc.what() << std::endl;
        }

        try {
            ysave_load::Save(
                web_graph_.Get(),
                config_.documents().documents_data_directory()
                + config_.documents().web_graph_file_name()
            );
        } catch (const std::exception& exc) {
            std::cerr << __FILE__ << ':' << __LINE__ << " EXCEPTION: " << exc.what() << std::endl;
        }

        // We write config as the last one
        try {
            yproto::WriteDelimitedToFile(config_, dir + config_.state().config_file_name());
        } catch (const std::exception& exc) {
            std::cerr << __FILE__ << ':' << __LINE__ << " EXCEPTION: " << exc.what() << std::endl;
        }
    }


    SimpleCrawlerConfig config_;

    bool should_save_ = false;
    bool valid_ = false;

    sci::UrlToId url_to_id_;
    sci::UrlsQueue urls_queue_;
    sci::VectorWithMutex<sci::url::UrlId> failed_urls_;
    sci::VectorWithMutex<sci::url::UrlId> processed_urls_;

    sci::SparceGraphWithMutex<sci::url::UrlId> web_graph_;

    sci::Coordnator coordinator_;
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
    if (processed_urls_.Size() >= 20) {
        return;
    }

//    static std::mutex lovely_mutex;

    const auto downloader = std::make_unique<ydownload::WgetDownloader>();
    const auto link_extractor = std::make_unique<ycrawler::SimpleWikipediaUrlExtractor>();
    const auto queue_response = urls_queue_.Pop();
    if (queue_response.empty) {
/*        {
            std::lock_guard<std::mutex> lock{lovely_mutex};
            std::cerr << "queue is empty" << std::endl;
        }*/
        return;
    }
    const auto url = url_to_id_.Get(queue_response.url_with_tries.id);
/*    {
        std::lock_guard<std::mutex> lock{lovely_mutex};
        std::cerr << "processing url: " << url << std::endl;
    }*/
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
/*    {
        std::lock_guard<std::mutex> lock{lovely_mutex};
        std::cerr << "queue size: " << urls_queue_.Size() << std::endl;
    }*/
}


void ycrawler::SimpleCrawler::Impl::StartImpl() {
    coordinator_.Run(
        [this](){ this->ProcessUrl(); },
        config_.threads(),
        [this](){ return this->urls_queue_.Empty(); }
    );
}

