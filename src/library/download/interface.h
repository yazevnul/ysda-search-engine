#pragma once

#include <string>

namespace ydownload {

    struct Response {
        bool Success;

        // Some day there would be response code
    };


    class IDownloader {
    public:
        virtual Response Download(const std::string& url, const std::string& output_file_name) = 0;

        virtual ~IDownloader() = 0;
    };


    inline IDownloader::~IDownloader() {
    }

}  // namespace ydownload

