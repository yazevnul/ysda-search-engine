#pragma once

#include "crawler_interface.h"

#include <memory>
#include <string>
#include <vector>

#include <cstdint>

// forward declarations BEGIN

namespace ydownload {

    class IDownloader;

}  // namespace ydownload


// forward declarations END


namespace ycrawler {

    class SimpleWikipediaCrawler: public ICrawler {
    public:
        struct Config {
            std::uint32_t threads = 4;
            std::string state_directory;
            std::string documents_directory;
            std::string documents_data_directory;
            std::vector<std::string> urls_seed;
        };

        SimpleWikipediaCrawler();
        virtual ~SimpleWikipediaCrawler();

        void SetConfig(const Config& config);

        void SetDownloader(std::unique_ptr<ydownload::IDownloader>&& downloder);

        void Restore(const std::string& state_directory);

        virtual void Start() override;

        // Will do nothing
        virtual void Stop() override;

    private:
        class Impl;

        std::unique_ptr<Impl> pimpl_;
    };

}  // namespace ycrawler

