#include <asterkv/logging/logger.h>

#include <spdlog/logger.h>
#include <spdlog/sinks/ostream_sink.h>

#include <memory>
#include <sstream>
#include <string>

namespace {

[[nodiscard]] AsterKV::Logging::LoggerPtr makeTestLogger(
    std::ostringstream& output,
    std::string_view name) {
    auto sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(output);
    auto logger = std::make_shared<spdlog::logger>(std::string{name}, sink);

    logger->set_pattern("[%l] %v");
    logger->set_level(spdlog::level::debug);

    return logger;
}

[[nodiscard]] bool testConvertsLogLevelsToStrings() {
    return AsterKV::Logging::logLevelToString(AsterKV::Logging::LogLevel::Debug) == "debug" &&
           AsterKV::Logging::logLevelToString(AsterKV::Logging::LogLevel::Info) == "info" &&
           AsterKV::Logging::logLevelToString(AsterKV::Logging::LogLevel::Warn) == "warn" &&
           AsterKV::Logging::logLevelToString(AsterKV::Logging::LogLevel::Error) == "error" &&
           AsterKV::Logging::logLevelToString(AsterKV::Logging::LogLevel::Critical) == "critical" &&
           AsterKV::Logging::logLevelToString(AsterKV::Logging::LogLevel::Off) == "off";
}

[[nodiscard]] bool testParsesLogLevels() {
    auto debug = AsterKV::Logging::logLevelFromString("debug");
    auto info = AsterKV::Logging::logLevelFromString("INFO");
    auto warn = AsterKV::Logging::logLevelFromString("warning");
    auto error = AsterKV::Logging::logLevelFromString("error");
    auto critical = AsterKV::Logging::logLevelFromString("critical");
    auto off = AsterKV::Logging::logLevelFromString("off");
    auto invalid = AsterKV::Logging::logLevelFromString("verbose");

    return debug.isOk() &&
           info.isOk() &&
           warn.isOk() &&
           error.isOk() &&
           critical.isOk() &&
           off.isOk() &&
           invalid.isError() &&
           debug.value() == AsterKV::Logging::LogLevel::Debug &&
           info.value() == AsterKV::Logging::LogLevel::Info &&
           warn.value() == AsterKV::Logging::LogLevel::Warn &&
           error.value() == AsterKV::Logging::LogLevel::Error &&
           critical.value() == AsterKV::Logging::LogLevel::Critical &&
           off.value() == AsterKV::Logging::LogLevel::Off;
}

[[nodiscard]] bool testWritesThroughSpecificLogger() {
    std::ostringstream output;
    auto logger = makeTestLogger(output, "asterkv_logging_specific_test");

    AsterKV::Logging::info(logger, "server started");
    AsterKV::Logging::warn(logger, "client rejected");

    logger->flush();

    return output.str() ==
           "[info] server started\n"
           "[warning] client rejected\n";
}

[[nodiscard]] bool testFiltersMessagesBelowMinLevel() {
    std::ostringstream output;
    auto logger = makeTestLogger(output, "asterkv_logging_filter_test");

    AsterKV::Logging::setLoggerLevel(logger, AsterKV::Logging::LogLevel::Warn);

    AsterKV::Logging::debug(logger, "debug message");
    AsterKV::Logging::info(logger, "info message");
    AsterKV::Logging::warn(logger, "warn message");
    AsterKV::Logging::error(logger, "error message");

    logger->flush();

    return output.str() ==
           "[warning] warn message\n"
           "[error] error message\n";
}

[[nodiscard]] bool testDefaultLoggerFacade() {
    std::ostringstream output;
    auto logger = makeTestLogger(output, "asterkv_logging_default_test");

    AsterKV::Logging::setDefaultLogger(logger);

    AsterKV::Logging::info("default server started");
    AsterKV::Logging::error("default server failed");

    logger->flush();

    AsterKV::Logging::clearDefaultLogger();

    return output.str() ==
           "[info] default server started\n"
           "[error] default server failed\n";
}

[[nodiscard]] bool testNullLoggerDoesNothing() {
    AsterKV::Logging::clearDefaultLogger();

    AsterKV::Logging::info("this should not crash");
    AsterKV::Logging::error("this should not crash either");

    return true;
}

} // namespace

int main() {
    if (!testConvertsLogLevelsToStrings()) {
        return 1;
    }

    if (!testParsesLogLevels()) {
        return 1;
    }

    if (!testWritesThroughSpecificLogger()) {
        return 1;
    }

    if (!testFiltersMessagesBelowMinLevel()) {
        return 1;
    }

    if (!testDefaultLoggerFacade()) {
        return 1;
    }

    if (!testNullLoggerDoesNothing()) {
        return 1;
    }

    return 0;
}
