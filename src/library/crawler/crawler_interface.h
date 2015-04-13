#pragma once

#include <string>


namespace ycrawler {

    class ICrawler {
    public:
        virtual void Start() = 0;
        virtual void Stop() = 0;

        virtual ~ICrawler() {
        }
    };

}  // namespace ycrawler

