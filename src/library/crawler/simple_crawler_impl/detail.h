#pragma once

#include <mutex>

namespace ycrawler {

    namespace sci {

        class WithMutex {
        public:
            virtual ~WithMutex() noexcept = 0;

        protected:
            mutable std::mutex object_mutex_;
        };


        inline WithMutex::~WithMutex() noexcept {
        }

    }  // namespace sci

}  // namespace ycrawler
