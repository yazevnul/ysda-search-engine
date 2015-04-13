#include "simple_crawler.h"
#include "url_extractor.h"

#include <library/download/interface.h>

#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <cstdint>


class ycrawler::SimpleCrawler::Impl {
public:
    void SetDownloader(std::unique_ptr<ydownload::IDownloader>&& downloder) {
        throw std::runtime_error{"Unimplemented"};
    }

    void SetConfig(const Config& config) {
        config_ = config;
    }

    void Start() {
        throw std::runtime_error{"Unimplemented"};
    }

    void Stop() {
    }

    void Restore(const std::string& state_directory) {
        throw std::runtime_error{"Unimplemented"};
    }

    void MoveFailedToQueue() {
        throw std::runtime_error{"Unimplemented"};
    }

private:
    Config config_;
};


// ycrawler::SimpleCrawler implementation BEGIN

void ycrawler::SimpleCrawler::SetDownloader(
    std::unique_ptr<ydownload::IDownloader>&& downloader
){
    pimpl_->SetDownloader(std::forward<std::unique_ptr<ydownload::IDownloader>>(downloader));
}


void ycrawler::SimpleCrawler::SetConfig(const Config& config) {
    pimpl_->SetConfig(config);
}


void ycrawler::SimpleCrawler::Start() {
    pimpl_->Start();
}


void ycrawler::SimpleCrawler::Stop() {
    pimpl_->Stop();
}


void ycrawler::SimpleCrawler::Restore(const std::string& state_directory) {
    pimpl_->Restore(state_directory);
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
