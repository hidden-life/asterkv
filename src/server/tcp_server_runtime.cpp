#include <asterkv/server/tcp_server_runtime.h>
#include <asterkv/server/signal_shutdown_controller.h>
#include <asterkv/logging/logger.h>

#include <utility>

namespace AsterKV::Server {
    TcpServerRuntime::TcpServerRuntime(TcpServerOptions options):
        options_(std::move(options)),
        storage_(),
        pipeline_(storage_),
        server_(
            options_.endpoint,
            pipeline_,
            Network::TcpLineServerOptions {
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
        Logging::info("installing TCP server shutdown signal handlers");

        Core::Status signalStatus = SignalShutdownController::install();

        if (!signalStatus.isOk()) {
            Logging::error("failed to install TCP server shutdown signal handlers");

            return signalStatus;
        }

        Logging::info("starting TCP server runtime");

        Core::Status status = server_.run(SignalShutdownController::stopRequested);

        if (!status.isOk()) {
            std::string message = "TCP server runtime stopped with error: ";
            message.append(status.codeString());
            message.push_back(' ');
            message.append(status.message());

            Logging::error(message);

            return status;
        }

        Logging::info("TCP server runtime stopped");

        return Core::Status::ok();
    }
}
