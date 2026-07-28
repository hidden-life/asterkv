#ifndef ASTERKV_NETWORK_TCP_SERVER_H
#define ASTERKV_NETWORK_TCP_SERVER_H

#include <asterkv/core/status.h>
#include <asterkv/network/tcp_endpoint.h>
#include <asterkv/pipeline/local_pipeline.h>

#include <cstddef>
#include <cstdint>

namespace AsterKV::Network {
    constexpr std::size_t defaultMaxClientWorkers = 128;
    constexpr std::uint32_t defaultClientIdleTimeoutSeconds = 300;

    struct TcpLineServerOptions final {
        std::size_t maxClientWorkers = defaultMaxClientWorkers;
        std::uint32_t clientIdleTimeoutSeconds = defaultClientIdleTimeoutSeconds;
    };

    class TcpLineServer final {
    public:
        using StopRequestedCallback = bool (*)() noexcept;

        TcpLineServer(TcpEndpoint endpoint, Pipeline::LocalPipeline &pipeline, TcpLineServerOptions options = {});

        [[nodiscard]] const TcpEndpoint &endpoint() const noexcept;
        [[nodiscard]] const TcpLineServerOptions &options() const noexcept;

        [[nodiscard]] Core::Status runOnce();
        [[nodiscard]] Core::Status run(StopRequestedCallback stopRequested);

    private:
        TcpEndpoint endpoint_;
        Pipeline::LocalPipeline &pipeline_;
        TcpLineServerOptions options_;
    };
}

#endif //ASTERKV_NETWORK_TCP_SERVER_H
