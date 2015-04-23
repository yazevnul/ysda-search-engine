#pragma once

#include "interface.h"

#include <fstream>
#include <memory>
#include <mutex>
#include <ostream>
#include <string>


namespace ylogger {

    class Logger : public ILogger {
    public:
        // Logger instance will not own stream
        explicit Logger(std::ostream* const output, const bool flush_on_each_message = false)
            : mutex_{}
            , flush_on_each_message_{flush_on_each_message}
            , output_owner_{}
            , output_{output} {
        }
        explicit Logger(const std::string& file_name, const bool flush_on_each_message = false)
            : mutex_{}
            , flush_on_each_message_{flush_on_each_message}
            , output_owner_{std::make_unique<std::ofstream>(file_name)}
            , output_{output_owner_.get()} {
        }

        void SetFlushOnEachMessage(const bool value);

        virtual void Info(const std::string& message) override;
        virtual void Debug(const std::string& message) override;
        virtual void Error(const std::string& message) override;
        virtual void Critical(const std::string& message) override;

        virtual void Write(const std::string& message, const Level level) override;

        virtual ~Logger() {
        }

    private:
        mutable std::mutex mutex_;
        bool flush_on_each_message_ = false;
        std::unique_ptr<std::ostream> output_owner_;
        std::ostream* const output_;
    };


    // will return Logger wrapper for std::clog which would flush on each message
    const std::shared_ptr<ILogger>& GetClog();

}  // namespace ylogger
