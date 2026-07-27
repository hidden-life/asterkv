#include <asterkv/network/tcp_endpoint.h>
#include <asterkv/server/signal_shutdown_controller.h>
#include <asterkv/server/tcp_server_options.h>
#include <asterkv/server/tcp_server_runtime.h>

#include <cstdint>
#include <string>

namespace {

    [[nodiscard]] bool expectEqual(std::string_view left, std::string_view right) {
        return left == right;
    }

    [[nodiscard]] bool expectEqual(std::uint16_t left, std::uint16_t right) {
        return left == right;
    }

    [[nodiscard]] bool expectEqual(std::size_t left, std::size_t right) {
        return left == right;
    }

    [[nodiscard]] bool testCreatesDefaultOptions() {
        const AsterKV::Server::TcpServerOptions options =
            AsterKV::Server::defaultServerOptions();

        return expectEqual(options.endpoint.host, "127.0.0.1") &&
               expectEqual(options.endpoint.port, AsterKV::Network::defaultClientPort) &&
               expectEqual(options.maxClientWorkers, AsterKV::Network::defaultMaxClientWorkers);
    }

    [[nodiscard]] bool testRuntimeKeepsConfiguredEndpointAndLimits() {
        const AsterKV::Server::TcpServerOptions options{
            .endpoint = AsterKV::Network::TcpEndpoint{
                .host = "127.0.0.1",
                .port = std::uint16_t{17721},
            },
            .maxClientWorkers = std::size_t{16},
        };

        AsterKV::Server::TcpServerRuntime runtime{options};

        return expectEqual(runtime.endpoint().host, "127.0.0.1") &&
               expectEqual(runtime.endpoint().port, std::uint16_t{17721}) &&
               expectEqual(runtime.options().maxClientWorkers, std::size_t{16});
    }

    [[nodiscard]] bool testSignalShutdownControllerCanResetStopState() {
        AsterKV::Server::SignalShutdownController::reset();

        return !AsterKV::Server::SignalShutdownController::stopRequested();
    }

} // namespace

int main() {
    if (!testCreatesDefaultOptions()) {
        return 1;
    }

    if (!testRuntimeKeepsConfiguredEndpointAndLimits()) {
        return 1;
    }

    if (!testSignalShutdownControllerCanResetStopState()) {
        return 1;
    }

    return 0;
}
