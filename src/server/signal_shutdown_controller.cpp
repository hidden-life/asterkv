#include <asterkv/server/signal_shutdown_controller.h>

#include <cerrno>
#include <csignal>
#include <cstring>
#include <string>

namespace AsterKV::Server {
    namespace {
        volatile std::sig_atomic_t shutdownRequested = 0;

        void handleStopSignal(int /* signalNumber */) {
            shutdownRequested = 1;
        }

        [[nodiscard]] Core::Status installHandlerForSignal(int signalNumber, const char *signalName) {
            struct sigaction action {};
            action.sa_handler = handleStopSignal;
            action.sa_flags = 0;

            if (::sigemptyset(&action.sa_mask) != 0) {
                std::string message = "failed to initialize signal mask: ";
                message.append(std::strerror(errno));

                return Core::Status::unavailable(message);
            }

            if (::sigaction(signalNumber, &action, nullptr) != 0) {
                std::string message = "failed to install ";
                message.append(signalName);
                message.append(" handler: ");
                message.append(std::strerror(errno));

                return Core::Status::unavailable(message);
            }

            return Core::Status::ok();
        }
    }

    void SignalShutdownController::reset() noexcept {
        shutdownRequested = 0;
    }

    Core::Status SignalShutdownController::install() {
        reset();

        Core::Status sigintStatus = installHandlerForSignal(SIGINT, "SIGINT");
        if (!sigintStatus.isOk()) {
            return sigintStatus;
        }

        Core::Status sigtermStatus = installHandlerForSignal(SIGTERM, "SIGTERM");
        if (!sigtermStatus.isOk()) {
            return sigtermStatus;
        }

        return Core::Status::ok();
    }

    bool SignalShutdownController::stopRequested() noexcept {
        return shutdownRequested != 0;
    }
}
