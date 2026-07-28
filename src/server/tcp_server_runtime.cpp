#include <asterkv/server/tcp_server_runtime.h>
#include <asterkv/server/signal_shutdown_controller.h>

#include <utility>

namespace AsterKV::Server {
    TcpServerRuntime::TcpServerRuntime(TcpServerOptions options):
        options_(std::move(options)),
        storage_(),
        pipeline_(storage_),
        server_(options_.endpoint, pipeline_, Network::TcpLineServerOptions {
            .maxClientWorkers = options_.maxClientWorkers,
            .clientIdleTimeoutSeconds = options_.clientIdleTimeoutSeconds,
        }) {
    }

    const TcpServerOptions & TcpServerRuntime::options() const noexcept {
        return options_;
    }

    const Network::TcpEndpoint & TcpServerRuntime::endpoint() const noexcept {
        return server_.endpoint();
    }

    Core::Status TcpServerRuntime::run() {
        Core::Status signalStatus = SignalShutdownController::install();
        if (!signalStatus.isOk()) {
            return signalStatus;
        }

        return server_.run(SignalShutdownController::stopRequested);
    }
}
