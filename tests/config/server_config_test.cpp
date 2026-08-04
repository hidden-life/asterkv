#include <asterkv/config/server_config.h>
#include <asterkv/network/tcp_server.h>
#include <asterkv/logging/logger.h>

#include <cstdint>
#include <string>
#include <string_view>

namespace {

    [[nodiscard]] bool expectEqual(std::string_view left, std::string_view right) {
        return left == right;
    }

    [[nodiscard]] bool expectEqual(std::uint16_t left, std::uint16_t right) {
        return left == right;
    }

    [[nodiscard]] bool expectEqual(std::uint32_t left, std::uint32_t right) {
        return left == right;
    }

    [[nodiscard]] bool expectEqual(std::size_t left, std::size_t right) {
        return left == right;
    }

    [[nodiscard]] bool expectEqual(AsterKV::Logging::LogLevel left, AsterKV::Logging::LogLevel right) {
        return left == right;
    }

    [[nodiscard]] bool testEmptyConfigUsesDefaults() {
        auto result = AsterKV::Config::parseServerConfig("");

        if (result.isError()) {
            return false;
        }

        return expectEqual(result.value().endpoint.host, "127.0.0.1") &&
               expectEqual(result.value().endpoint.port, AsterKV::Network::defaultClientPort) &&
               expectEqual(result.value().maxClientWorkers, AsterKV::Network::defaultMaxClientWorkers) &&
               expectEqual(result.value().clientIdleTimeoutSeconds,
                           AsterKV::Network::defaultClientIdleTimeoutSeconds) &&
               expectEqual(result.value().logLevel, AsterKV::Logging::defaultLogLevel) &&
                expectEqual(result.value().walFilePath, "")
            ;
    }

    [[nodiscard]] bool testParsesFullConfig() {
        const std::string configText =
            "# comment\n"
            "listen = 127.0.0.1:17721\n"
            "max_clients = 16\n"
            "idle_timeout_seconds = 30\n"
            "log_level = debug\n"
            "wal_file = ./data/asterkv.wal\n"
            ;

        auto result = AsterKV::Config::parseServerConfig(configText);

        if (result.isError()) {
            return false;
        }

        return expectEqual(result.value().endpoint.host, "127.0.0.1") &&
               expectEqual(result.value().endpoint.port, std::uint16_t{17721}) &&
               expectEqual(result.value().maxClientWorkers, std::size_t{16}) &&
               expectEqual(result.value().clientIdleTimeoutSeconds, std::uint32_t{30}) &&
               expectEqual(result.value().logLevel, AsterKV::Logging::LogLevel::Debug) &&
                   expectEqual(result.value().walFilePath, "./data/asterkv.wal")
            ;
    }

    [[nodiscard]] bool testRejectsUnknownKey() {
        auto result = AsterKV::Config::parseServerConfig("unknown = value\n");

        return result.isError();
    }

    [[nodiscard]] bool testRejectsInvalidLine() {
        auto result = AsterKV::Config::parseServerConfig("listen 127.0.0.1:7721\n");

        return result.isError();
    }

    [[nodiscard]] bool testRejectsInvalidEndpoint() {
        auto result = AsterKV::Config::parseServerConfig("listen = invalid\n");

        return result.isError();
    }

    [[nodiscard]] bool testRejectsInvalidMaxClients() {
        auto result = AsterKV::Config::parseServerConfig("max_clients = 0\n");

        return result.isError();
    }

    [[nodiscard]] bool testRejectsInvalidIdleTimeout() {
        auto result = AsterKV::Config::parseServerConfig("idle_timeout_seconds = 0\n");

        return result.isError();
    }

    [[nodiscard]] bool testRejectsInvalidLogLevel() {
        auto result = AsterKV::Config::parseServerConfig("log_level = verbose\n");

        return result.isError();
    }

    [[nodiscard]] bool testRejectsEmptyWalFile() {
        auto result = AsterKV::Config::parseServerConfig("wal_file = \n");

        return result.isError();
    }

} // namespace

int main() {
    if (!testEmptyConfigUsesDefaults()) {
        return 1;
    }

    if (!testParsesFullConfig()) {
        return 1;
    }

    if (!testRejectsUnknownKey()) {
        return 1;
    }

    if (!testRejectsInvalidLine()) {
        return 1;
    }

    if (!testRejectsInvalidEndpoint()) {
        return 1;
    }

    if (!testRejectsInvalidMaxClients()) {
        return 1;
    }

    if (!testRejectsInvalidIdleTimeout()) {
        return 1;
    }

    if (!testRejectsInvalidLogLevel()) {
        return 1;
    }

    if (!testRejectsEmptyWalFile()) {
        return 1;
    }

    return 0;
}
