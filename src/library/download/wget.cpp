#include "interface.h"
#include "wget.h"

#include <cstdlib>
#include <stdexcept>
#include <string>


class CommandProcessorAvailabilityChecker {
public:
    inline CommandProcessorAvailabilityChecker()
        : available_(system(nullptr)) {
    }

    inline bool IsAvailable() {
        return available_;
    }

private:
    bool available_;
};


ydownload::WgetDownloader::WgetDownloader() {
    static auto checker = CommandProcessorAvailabilityChecker();
    if (! checker.IsAvailable())
        throw std::runtime_error("command processor is unavailable");
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

