#pragma once

#include <string>

#include <cstdint>


namespace ylogger {

    enum Level : std::uint32_t {
        INFO = 0,
        DEBUG = 1,
        ERROR = 2,
        CRITICAL = 3
    };


    class ILogger {
    public:
        virtual void Info(const std::string& message) = 0;
        virtual void Debug(const std::string& message) = 0;
        virtual void Error(const std::string& message) = 0;
        virtual void Critical(const std::string& message) = 0;

        virtual void Write(const std::string& message, const Level level) = 0;

        virtual ~ILogger() {
        }
    };

}  // namespace ylogger

#define LIBRARY_LOGGER_PREFIX                      \
    std::string{__FILE__} + ':' + __LINE__ + '\t'

#define LOG_INFO(logger_ptr, message)                   \
    logger_ptr->Info(LIBRARY_LOGGER_PREFIX + message)

#define LOG_DEBUG(logger_ptr, message)                   \
    logger_ptr->Debug(LIBRARY_LOGGER_PREFIX + message)

#define LOG_ERROR(logger_ptr, message)                   \
    logger_ptr->Error(LIBRARY_LOGGER_PREFIX + message)

#define LOG_CRITICAL(logger_ptr, message)                   \
    logger_ptr->Critical(LIBRARY_LOGGER_PREFIX + message)

#undef LIBRARY_LOGGER_PREFIX

#ifdef NDEBUG
#define LOG_IF_DEBUG(logger_ptr, message) \
    LOG_DEBUG(logger_ptr, message)
#else
#define LOG_IF_DEBUG(logger_ptr, message)
#endif
