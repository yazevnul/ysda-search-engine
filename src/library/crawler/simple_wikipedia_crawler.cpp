#include "simple_wikipedia_crawler.h"
#include "url_extractor.h"

#include <library/download/interface.h>

#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <cstdint>


class ycrawler::SimpleWikipediaCrawler::Impl {
public:
    void SetDownloader(std::unique_ptr<ydownload::IDownloader>&& downloder) {
        throw std::runtime_error("Unimplemented");
    }

    void SetConfig(const Config& config) {
        config_ = config;
    }

    void Start() {
        throw std::runtime_error("Unimplemented");
    }

    void Stop() {
    }

    void Restore(const std::string& state_directory) {
        throw std::runtime_error("Unimplemented");
    }

private:
    Config config_;
};


// ycrawler::SimpleWikipediaCrawler implementation BEGIN

void ycrawler::SimpleWikipediaCrawler::SetDownloader(
    std::unique_ptr<ydownload::IDownloader>&& downloader
){
    pimpl_->SetDownloader(std::forward<std::unique_ptr<ydownload::IDownloader>>(downloader));
}


void ycrawler::SimpleWikipediaCrawler::SetConfig(const Config& config) {
    pimpl_->SetConfig(config);
}


void ycrawler::SimpleWikipediaCrawler::Start() {
    pimpl_->Start();
}


void ycrawler::SimpleWikipediaCrawler::Restore(const std::string& state_directory) {
    pimpl_->Restore(state_directory);
}


// We implement constructor inside cpp file because of PImpl patter we use
ycrawler::SimpleWikipediaCrawler::SimpleWikipediaCrawler()
    : pimpl_{std::make_unique<Impl>()} {
}


// We implement destructor inside cpp file because of PImpl pattern we use
ycrawler::SimpleWikipediaCrawler::~SimpleWikipediaCrawler() {
}

// ycrawler::SimpleWikipediaCrawler implementation END
