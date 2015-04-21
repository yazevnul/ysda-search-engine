#include "simple_crawler.h"
#include "url_extractor.h"

#include <library/crawler/simple_crawler_impl/graph.h>
#include <library/crawler/simple_crawler_impl/urls.h>
#include <library/crawler/simple_crawler_impl/vector.h>
#include <library/download/interface.h>
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
        throw std::runtime_error{"Unimplemented"};
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
        if (valid_) {
            Save();
        }
    }

private:
    void Init() {
        for (const auto url: config_.urls_seed()) {
            const auto url_id = url_to_id_.Add(url);
            urls_queue_.Push({url_id, 0});
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

    bool valid_ = false;

    sci::UrlToId url_to_id_;
    sci::UrlsQueue urls_queue_;
    sci::VectorWithMutex<sci::url::UrlId> failed_urls_;
    sci::VectorWithMutex<sci::url::UrlId> processed_urls_;

    sci::SparceGraphWithMutex<sci::url::UrlId> web_graph_;
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
