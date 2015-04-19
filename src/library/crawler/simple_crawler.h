#pragma once

#include "crawler_interface.h"
#include "protos/simple_crawler_config.pb.h"

#include <memory>
#include <string>
#include <vector>

#include <cstdint>


namespace ycrawler {

    class SimpleCrawler: public ICrawler {
    public:
        SimpleCrawler();
        virtual ~SimpleCrawler();

        void SetConfig(const SimpleCrawlerConfig& config);

        void Restore(const std::string& config_file_name);

        void MoveFailedToQueue();

        virtual void Start() override;

        // Will do nothing
        virtual void Stop() override;

    private:
        class Impl;

        std::unique_ptr<Impl> pimpl_;
    };

}  // namespace ycrawler

