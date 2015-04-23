#include "logger.h"

#include <chrono>
#include <iostream>
#include <mutex>
#include <ostream>
#include <string>

#include <ctime>

const std::shared_ptr<ylogger::ILogger>& ylogger::GetClog() {
    static std::shared_ptr<ylogger::ILogger> clog = std::make_shared<ylogger::Logger>(
        &std::clog, true
    );
    return clog;
}


static constexpr char PREFIX_INFO[] = "INFO: ";
static constexpr char PREFIX_DEBUG[] = "DEBUG: ";
static constexpr char PREFIX_ERROR[] = "ERROR: ";
static constexpr char PREFIX_CRITICAL[] = "CRITICAL: ";


static std::string ToString(const std::time_t the_time) {
    // pretty boring
    char buffer[100];
    if (std::strftime(buffer, sizeof(buffer), "%F %T", std::gmtime(&the_time))) {
        return buffer;
    }
    return {};
}


static void WriteTime(std::ostream& output) {
    const auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    output << ToString(now) << '\t';
}


static void WriteInfo(const std::string& message, const bool flush, std::ostream& output) {
    WriteTime(output);
    output << PREFIX_INFO << message << '\n';
    if (flush) {
        output.flush();
    }
}


static void WriteDebug(const std::string& message, const bool flush, std::ostream& output) {
    WriteTime(output);
    output << PREFIX_DEBUG << message << '\n';
    if (flush) {
        output.flush();
    }
}


static void WriteError(const std::string& message, const bool flush, std::ostream& output) {
    WriteTime(output);
    output << PREFIX_ERROR << message << '\n';
    if (flush) {
        output.flush();
    }
}


static void WriteCritical(const std::string& message, const bool flush, std::ostream& output) {
    WriteTime(output);
    output << PREFIX_CRITICAL << message << '\n';
    if (flush) {
        output.flush();
    }
}


void ylogger::Logger::Info(const std::string& message) {
    std::lock_guard<std::mutex> lock_guard{mutex_};
    if (!output_) {
        return;
    }
    WriteInfo(message, flush_on_each_message_, *output_);
}


void ylogger::Logger::Debug(const std::string& message) {
    std::lock_guard<std::mutex> lock_guard{mutex_};
    if (!output_) {
        return;
    }
    WriteDebug(message, flush_on_each_message_, *output_);
}


void ylogger::Logger::Error(const std::string& message) {
    std::lock_guard<std::mutex> lock_guard{mutex_};
    if (!output_) {
        return;
    }
    WriteError(message, flush_on_each_message_, *output_);
}


void ylogger::Logger::Critical(const std::string& message) {
    std::lock_guard<std::mutex> lock_guard{mutex_};
    if (output_) {
        return;
    }
    WriteCritical(message, flush_on_each_message_, *output_);
}


void ylogger::Logger::Write(const std::string& message, const Level level) {
    std::lock_guard<std::mutex> lock_guard{mutex_};
    if (!output_) {
        return;
    }
    switch (level) {
        case INFO_LVL: {
            WriteInfo(message, flush_on_each_message_, *output_);
            break;
        } case DEBUG_LVL: {
            WriteDebug(message, flush_on_each_message_, *output_);
            break;
        } case ERROR_LVL: {
            WriteError(message, flush_on_each_message_, *output_);
            break;
        } case CRITICAL_LVL: {
            WriteCritical(message, flush_on_each_message_, *output_);
            break;
        }
    };
}


void ylogger::Logger::SetFlushOnEachMessage(const bool value) {
    std::lock_guard<std::mutex> lock_guard{mutex_};
    flush_on_each_message_ = value;
}

