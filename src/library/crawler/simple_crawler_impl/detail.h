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


        class WithLock {
        public:
            WithLock()
                : mutex_for_object_lock_{}
                , object_lock_{mutex_for_object_lock_} {
            }

            virtual ~WithLock() noexcept  = 0;

        private:
            std::mutex mutex_for_object_lock_;

        protected:
            mutable std::unique_lock<std::mutex> object_lock_;
        };


        inline WithLock::~WithLock() noexcept {
        }

    }  // namespace sci

}  // namespace ycrawler
