#ifndef ASTERKV_NETWORK_TCP_SERVER_H
#define ASTERKV_NETWORK_TCP_SERVER_H

#include <asterkv/core/status.h>
#include <asterkv/network/tcp_endpoint.h>
#include <asterkv/pipeline/local_pipeline.h>

namespace AsterKV::Network {
    class TcpLineServer final {
    public:
        using StopRequestedCallback = bool (*)() noexcept;

        TcpLineServer(TcpEndpoint endpoint, Pipeline::LocalPipeline &pipeline);

        [[nodiscard]] const TcpEndpoint &endpoint() const noexcept;
        [[nodiscard]] Core::Status runOnce();

        [[nodiscard]] Core::Status run(StopRequestedCallback stopRequested);

    private:
        TcpEndpoint endpoint_;
        Pipeline::LocalPipeline &pipeline_;
    };
}

#endif //ASTERKV_NETWORK_TCP_SERVER_H
