#ifndef ASTERKV_SERVER_SIGNAL_SHUTDOWN_CONTROLLER_H
#define ASTERKV_SERVER_SIGNAL_SHUTDOWN_CONTROLLER_H

#include <asterkv/core/status.h>

namespace AsterKV::Server {
    class SignalShutdownController final {
    public:
        SignalShutdownController() = delete;

        static void reset() noexcept;

        [[nodiscard]] static Core::Status install();
        [[nodiscard]] static bool stopRequested() noexcept;
    };
}

#endif //ASTERKV_SERVER_SIGNAL_SHUTDOWN_CONTROLLER_H
