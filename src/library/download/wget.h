#pragma once

#include "interface.h"

#include <string>

namespace ydownload {

    class WgetDownloader : public IDownloader {
    public:
        WgetDownloader();

        virtual Response Download(
            const std::string& url, const std::string& output_file_name
        ) override;

        virtual ~WgetDownloader() {
        }
    };

}  // namesapce ydownload

