#include <asterkv/server/tcp_server_runtime.h>
#include <asterkv/server/signal_shutdown_controller.h>
#include <asterkv/logging/logger.h>

#include <utility>

namespace AsterKV::Server {
    namespace {
        [[nodiscard]] bool isWalEnabled(const TcpServerOptions &options) noexcept {
            return !options.walFilePath.empty();
        }

        [[nodiscard]] std::unique_ptr<Wal::WalBackedStorage> makeWalStorage(
            Storage::InMemoryStorage &storage,
            const TcpServerOptions &options
        ) {
            if (!isWalEnabled(options)) {
                return nullptr;
            }

            return std::make_unique<Wal::WalBackedStorage>(storage, options.walFilePath, Wal::WalBackedStorageOptions {
                .writerOptions = Wal::WalFileWriterOptions {
                    .syncPolicy = options.walSyncPolicy,
                }
            });
        }

        [[nodiscard]] Storage::StorageEngine &selectPipelineStorage(
            Storage::InMemoryStorage &storage,
            const std::unique_ptr<Wal::WalBackedStorage> &walStorage
            ) noexcept {
            if (walStorage) {
                return *walStorage;
            }

            return storage;
        }
    }

    TcpServerRuntime::TcpServerRuntime(TcpServerOptions options):
        options_(std::move(options)),
        storage_(),
        walStorage_(makeWalStorage(storage_, options_)),
        pipeline_(selectPipelineStorage(storage_, walStorage_)),
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
        if (walStorage_) {
            std::string message = "recovering storage from WAL file: ";
            message.append(walStorage_->filePath());

            Logging::info(message);

            const Core::Status recoveryStatus = walStorage_->recover();
            if (!recoveryStatus.isOk()) {
                Logging::error("WAL recovery failed");

                return recoveryStatus;
            }

            Logging::info("WAL recovery completed");
        }

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
