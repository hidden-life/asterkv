#include <charconv>
#include <fstream>
#include <sstream>

#include <asterkv/config/server_config.h>
#include <asterkv/logging/logger.h>

namespace AsterKV::Config {
    namespace {
        [[nodiscard]] bool isAsciiSpace(char ch) noexcept {
            return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
        }

        [[nodiscard]] std::string_view trim(std::string_view value) noexcept {
            while (!value.empty() && isAsciiSpace(value.front())) {
                value.remove_prefix(1);
            }

            while (!value.empty() && isAsciiSpace(value.back())) {
                value.remove_suffix(1);
            }

            return value;
        }

        [[nodiscard]] bool isCommentOrEmptyLine(std::string_view line) noexcept {
            const std::string_view trimmed = trim(line);

            return trimmed.empty() || trimmed.front() == '#';
        }

        [[nodiscard]] Core::Status makeLineError(std::size_t lineNUmber, std::string_view message) {
            std::string errorMessage = "line ";
            errorMessage.append(std::to_string(lineNUmber));
            errorMessage.append(": ");
            errorMessage.append(message);

            return Core::Status::invalidArgument(std::move(errorMessage));
        }

        [[nodiscard]] Core::Result<std::size_t> parsePositiveSizeValue(
            std::string_view value,
            std::size_t lineNumber,
            std::string_view fieldName) {
            value = trim(value);

            std::size_t parsedValue = 0;

            const char* const begin = value.data();
            const char* const end = value.data() + value.size();

            const auto result = std::from_chars(begin, end, parsedValue);

            if (result.ec != std::errc {} || result.ptr != end || parsedValue == 0) {
                std::string message;
                message.append(fieldName);
                message.append(" must be a positive integer");

                return Core::Result<std::size_t>::failure(makeLineError(lineNumber, message));
            }

            return Core::Result<std::size_t>::success(parsedValue);
        }

        [[nodiscard]] Core::Result<std::uint32_t> parsePositiveUint32Value(
            std::string_view value,
            std::size_t lineNumber,
            std::string_view fieldName
        ) {
            auto parsedSize = parsePositiveSizeValue(value, lineNumber, fieldName);
            if (parsedSize.isError()) {
                return Core::Result<std::uint32_t>::failure(parsedSize.status());
            }

            if (parsedSize.value() > std::numeric_limits<std::uint32_t>::max()) {
                std::string message;
                message.append(fieldName);
                message.append(" is too large");

                return Core::Result<std::uint32_t>::failure(makeLineError(lineNumber, message));
            }

            return Core::Result<std::uint32_t>::success(static_cast<std::uint32_t>(parsedSize.value()));
        }

        [[nodiscard]] Core::Status applyConfigEntry(
            Server::TcpServerOptions &options,
            std::string_view key,
            std::string_view value,
            std::size_t lineNumber
        ) {
            key = trim(key);
            value = trim(value);

            if (key.empty()) {
                return makeLineError(lineNumber, "config key must not be empty");
            }

            if (key == "listen") {
                auto endpoint = Network::parseTcpEndpoint(value);
                if (endpoint.isError()) {
                    std::string message = "invalid listen endpoint: ";
                    message.append(endpoint.status().message());

                    return makeLineError(lineNumber, message);
                }

                options.endpoint = endpoint.value();

                return Core::Status::ok();
            }

            if (key == "max_clients") {
                auto parsed = parsePositiveSizeValue(value, lineNumber, "max_clients");
                if (parsed.isError()) {
                    return parsed.status();
                }

                options.maxClientWorkers = parsed.value();

                return Core::Status::ok();
            }

            if (key == "idle_timeout_seconds") {
                auto parsed = parsePositiveUint32Value(value, lineNumber, "idle_timeout_seconds");
                if (parsed.isError()) {
                    return parsed.status();
                }

                options.clientIdleTimeoutSeconds = parsed.value();

                return Core::Status::ok();
            }

            if (key == "log_level") {
                auto parsed = Logging::logLevelFromString(value);
                if (parsed.isError()) {
                    std::string message = "invalid log_level: ";
                    message.append(parsed.status().message());

                    return makeLineError(lineNumber, message);
                }

                options.logLevel = parsed.value();

                return Core::Status::ok();
            }

            if (key == "wal_file") {
                if (value.empty()) {
                    return makeLineError(lineNumber, "wal_file must not be empty");
                }

                options.walFilePath = std::string {value};

                return Core::Status::ok();
            }

            std::string message = "unknown config key: ";
            message.append(key);

            return makeLineError(lineNumber, message);
        }

        [[nodiscard]] Core::Status parseConfigLine(
            Server::TcpServerOptions &options,
            std::string_view line,
            std::size_t lineNumber
        ) {
            if (isCommentOrEmptyLine(line)) {
                return Core::Status::ok();
            }

            const std::size_t separatorPos = line.find('=');
            if (separatorPos == std::string_view::npos) {
                return makeLineError(lineNumber, "expected key = value");
            }

            const std::string_view key = line.substr(0, separatorPos);
            const std::string_view value = line.substr(separatorPos + 1);

            return applyConfigEntry(options, key, value, lineNumber);
        }
    }

    Core::Result<Server::TcpServerOptions> parseServerConfig(std::string_view config) {
        Server::TcpServerOptions options = Server::defaultServerOptions();
        std::size_t lineNumber = 1;
        std::size_t lineStart = 0;

        while (lineStart <= config.size()) {
            const std::size_t lineEnd = config.find('\n', lineStart);
            const std::string_view line = lineEnd == std::string_view::npos ?
                config.substr(lineStart) :
                config.substr(lineStart, lineEnd - lineStart);

            Core::Status status = parseConfigLine(options, line, lineNumber);
            if (!status.isOk()) {
                return Core::Result<Server::TcpServerOptions>::failure(status);
            }

            if (lineEnd == std::string_view::npos) {
                break;
            }

            lineStart = lineEnd + 1;
            ++lineNumber;
        }

        return Core::Result<Server::TcpServerOptions>::success(options);
    }

    Core::Result<Server::TcpServerOptions> loadServerConfig(std::string_view path) {
        std::ifstream input {std::string {path}};
        if (!input.is_open()) {
            std::string message = "failed to open config file: ";
            message.append(path);

            return Core::Result<Server::TcpServerOptions>::failure(
                Core::Status::unavailable(std::move(message))
            );
        }

        std::ostringstream buffer;
        buffer << input.rdbuf();

        if (input.bad()) {
            std::string message = "failed to read config file: ";
            message.append(path);

            return Core::Result<Server::TcpServerOptions>::failure(
                Core::Status::unavailable(std::move(message))
            );
        }

        return parseServerConfig(buffer.str());
    }
}
