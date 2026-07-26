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

    [[nodiscard]] bool testCreatesDefaultOptions() {
        const AsterKV::Server::TcpServerOptions options =
            AsterKV::Server::defaultServerOptions();

        return expectEqual(options.endpoint.host, "127.0.0.1") &&
               expectEqual(options.endpoint.port, AsterKV::Network::defaultClientPort);
    }

    [[nodiscard]] bool testRuntimeKeepsConfiguredEndpoint() {
        const AsterKV::Server::TcpServerOptions options{
            .endpoint = AsterKV::Network::TcpEndpoint{
                .host = "127.0.0.1",
                .port = std::uint16_t{17721},
            },
        };

        AsterKV::Server::TcpServerRuntime runtime{options};

        return expectEqual(runtime.endpoint().host, "127.0.0.1") &&
               expectEqual(runtime.endpoint().port, std::uint16_t{17721});
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

    if (!testRuntimeKeepsConfiguredEndpoint()) {
        return 1;
    }

    if (!testSignalShutdownControllerCanResetStopState()) {
        return 1;
    }

    return 0;
}
