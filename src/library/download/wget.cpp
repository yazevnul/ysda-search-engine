#include "wget.h"

#include <stdexcept>
#include <string>

#include <cstdlib>

static const bool COMMAND_PROCESSOR_EXISTS = system(nullptr);


ydownload::WgetDownloader::WgetDownloader() {
    if (!COMMAND_PROCESSOR_EXISTS) {
        throw std::runtime_error{"command processor is unavailable"};
    }
}


ydownload::Response ydownload::WgetDownloader::Download(
    const std::string& url, const std::string& output_file_name
){
    const std::string command = std::string("wget '") + url + "'"
        + " --output-document '" + output_file_name + "'"
        + " --quiet";
    int rv = system(command.c_str());
    if (rv)
        return { false };
    else
        return { true };
}

