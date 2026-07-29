#include <asterkv/logging/logger.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace AsterKV::Logging {
    namespace {
        [[nodiscard]] bool equalsIgnoreCase(std::string_view left, std::string_view right) noexcept {
            if (left.size() != right.size()) {
                return false;
            }

            for (std::size_t index = 0; index < left.size(); ++index) {
                const char leftCh = left[index];
                const char rightCh = right[index];
                const char normalizedLeft = leftCh >= 'A' && leftCh <= 'Z' ? static_cast<char>(leftCh - 'A' + 'a') : leftCh;
                const char normalizedRight = rightCh >= 'A' && rightCh <= 'Z' ? static_cast<char>(rightCh - 'A' + 'a') : rightCh;

                if (normalizedLeft != normalizedRight) {
                    return false;
                }
            }

            return true;
        }

        [[nodiscard]] spdlog::level::level_enum toSpdlogLevel(LogLevel level) noexcept {
            switch (level) {
                case LogLevel::Debug:
                    return spdlog::level::debug;

                case LogLevel::Error:
                    return spdlog::level::err;

                case LogLevel::Info:
                    return spdlog::level::info;

                case LogLevel::Warn:
                    return spdlog::level::warn;

                case LogLevel::Critical:
                    return spdlog::level::critical;

                case LogLevel::Off:
                    return spdlog::level::off;
            }

            return spdlog::level::info;
        }

        [[nodiscard]] std::mutex &defaultLoggerMtx() {
            static std::mutex mtx;

            return mtx;
        }

        [[nodiscard]] LoggerPtr &defaultStorage() {
            static LoggerPtr logger;

            return logger;
        }

        [[nodiscard]] spdlog::string_view_t toSpdlogStringView(std::string_view str) noexcept {
            return spdlog::string_view_t {str.data(), str.size()};
        }
    }

    std::string_view logLevelToString(LogLevel level) noexcept {
        switch (level) {
            case LogLevel::Debug:
                return "debug";
            case LogLevel::Info:
                return "info";
            case LogLevel::Warn:
                return "warn";
            case LogLevel::Error:
                return "error";
            case LogLevel::Critical:
                return "critical";
            case LogLevel::Off:
                return "off";
        }

        return "unknown";
    }

    Core::Result<LogLevel> logLevelFromString(std::string_view level) {
        if (equalsIgnoreCase(level, "debug")) {
            return Core::Result<LogLevel>::success(LogLevel::Debug);
        }

        if (equalsIgnoreCase(level, "info")) {
            return Core::Result<LogLevel>::success(LogLevel::Info);
        }

        if (equalsIgnoreCase(level, "warn") || equalsIgnoreCase(level, "warning")) {
            return Core::Result<LogLevel>::success(LogLevel::Warn);
        }

        if (equalsIgnoreCase(level, "error")) {
            return Core::Result<LogLevel>::success(LogLevel::Error);
        }

        if (equalsIgnoreCase(level, "critical")) {
            return Core::Result<LogLevel>::success(LogLevel::Critical);
        }

        if (equalsIgnoreCase(level, "off")) {
            return Core::Result<LogLevel>::success(LogLevel::Off);
        }

        return Core::Result<LogLevel>::failure(Core::Status::invalidArgument("unknown log level"));
    }

    LoggerPtr createConsoleLogger(std::string_view name, LogLevel level) {
        const std::string loggerName {name};
        LoggerPtr logger = spdlog::get(loggerName);

        if (logger == nullptr) {
            logger = spdlog::stderr_color_mt(loggerName);
        }

        logger->set_level(toSpdlogLevel(level));
        logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%n] %v");
        logger->flush_on(spdlog::level::warn);

        return logger;
    }

    void setLoggerLevel(const LoggerPtr &logger, LogLevel level) {
        if (logger != nullptr) {
            logger->set_level(toSpdlogLevel(level));
        }
    }

    void setDefaultLogger(LoggerPtr logger) {
        std::lock_guard lock {defaultLoggerMtx()};

        defaultStorage() = std::move(logger);
    }

    LoggerPtr defaultLogger() {
        std::lock_guard lock {defaultLoggerMtx()};

        return defaultStorage();
    }

    void clearDefaultLogger() {
        setDefaultLogger(nullptr);
    }

    void initializeDefaultConsoleLogger(std::string_view name, LogLevel level) {
        setDefaultLogger(createConsoleLogger(name, level));
    }

    void shutdownLogging() {
        clearDefaultLogger();

        spdlog::shutdown();
    }

    void log(const LoggerPtr &logger, LogLevel level, std::string_view message) {
        if (logger == nullptr) {
            return;
        }

        const spdlog::level::level_enum logLevel = toSpdlogLevel(level);
        if (!logger->should_log(logLevel)) {
            return;
        }

        logger->log(logLevel, toSpdlogStringView(message));
    }

    void debug(const LoggerPtr &logger, std::string_view message) {
        log(logger, LogLevel::Debug, message);
    }

    void info(const LoggerPtr &logger, std::string_view message) {
        log(logger, LogLevel::Info, message);
    }

    void warn(const LoggerPtr &logger, std::string_view message) {
        log(logger, LogLevel::Warn, message);
    }

    void error(const LoggerPtr &logger, std::string_view message) {
        log(logger, LogLevel::Error, message);
    }

    void critical(const LoggerPtr &logger, std::string_view message) {
        log(logger, LogLevel::Critical, message);
    }

    void log(LogLevel level, std::string_view message) {
        log(defaultLogger(), level, message);
    }

    void debug(std::string_view message) {
        debug(defaultLogger(), message);
    }

    void info(std::string_view message) {
        info(defaultLogger(), message);
    }

    void warn(std::string_view message) {
        warn(defaultLogger(), message);
    }

    void error(std::string_view message) {
        error(defaultLogger(), message);
    }

    void critical(std::string_view message) {
        critical(defaultLogger(), message);
    }
}
