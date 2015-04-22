#pragma once

#include <string>

#include <cstdint>


namespace ylogger {

    enum Level : std::uint32_t {
        INFO_LVL = 0,
        DEBUG_LVL = 1,
        ERROR_LVL = 2,
        CRITICAL_LVL = 3
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


#define LOG_INFO(logger_ptr, message)                   \
    logger_ptr->Info(std::string{__FILE__} + ':' + std::to_string(__LINE__) + '\t' + message)

#define LOG_DEBUG(logger_ptr, message)                   \
    logger_ptr->Debug(std::string{__FILE__} + ':' + std::to_string(__LINE__) + '\t' + message)

#define LOG_ERROR(logger_ptr, message)                   \
    logger_ptr->Error(std::string{__FILE__} + ':' + std::to_string(__LINE__) + '\t' + message)

#define LOG_CRITICAL(logger_ptr, message)                   \
    logger_ptr->Critical(std::string{__FILE__} + ':' + std::to_string(__LINE__) + '\t' + message)

#undef LIBRARY_LOGGER_PREFIX

#ifdef DEBUG
#define LOG_IF_DEBUG(logger_ptr, message) \
    LOG_DEBUG(logger_ptr, message)
#else
#define LOG_IF_DEBUG(logger_ptr, message)
#endif
