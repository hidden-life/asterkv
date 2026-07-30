#ifndef ASTERKV_LOGGING_LOGGER_H
#define ASTERKV_LOGGING_LOGGER_H

#include <memory>
#include <string_view>

#include <asterkv/core/result.h>

namespace spdlog {
    class logger;
}

namespace AsterKV::Logging {
    enum class LogLevel {
        Debug,
        Info,
        Warn,
        Error,
        Critical,
        Off,
    };

    constexpr LogLevel defaultLogLevel = LogLevel::Info;

    using LoggerPtr = std::shared_ptr<spdlog::logger>;

    [[nodiscard]] std::string_view logLevelToString(LogLevel level) noexcept;
    [[nodiscard]] Core::Result<LogLevel> logLevelFromString(std::string_view level);

    [[nodiscard]] LoggerPtr createConsoleLogger(std::string_view name, LogLevel level = LogLevel::Info);

    void setLoggerLevel(const LoggerPtr &logger, LogLevel level);

    void setDefaultLogger(LoggerPtr logger);
    [[nodiscard]] LoggerPtr defaultLogger();
    void clearDefaultLogger();

    void initializeDefaultConsoleLogger(std::string_view name, LogLevel level = LogLevel::Info);

    void shutdownLogging();

    void log(const LoggerPtr &logger, LogLevel level, std::string_view message);
    void debug(const LoggerPtr &logger, std::string_view message);
    void info(const LoggerPtr &logger, std::string_view message);
    void warn(const LoggerPtr &logger, std::string_view message);
    void error(const LoggerPtr &logger, std::string_view message);
    void critical(const LoggerPtr &logger, std::string_view message);

    void log(LogLevel level, std::string_view message);
    void debug(std::string_view message);
    void info(std::string_view message);
    void warn(std::string_view message);
    void error(std::string_view message);
    void critical(std::string_view message);
}

#endif //ASTERKV_LOGGING_LOGGER_H
