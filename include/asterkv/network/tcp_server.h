#ifndef ASTERKV_NETWORK_TCP_SERVER_H
#define ASTERKV_NETWORK_TCP_SERVER_H

#include <asterkv/network/tcp_endpoint.h>
#include <asterkv/network/tcp_server.h>
#include <asterkv/pipeline/local_pipeline.h>

namespace AsterKV::Network {
    class TcpLineServer final {
    public:
        TcpLineServer(TcpEndpoint endpoint, Pipeline::LocalPipeline &pipeline);

        [[nodiscard]] const TcpEndpoint &endpoint() const noexcept;
        [[nodiscard]] Core::Status runOnce();

    private:
        TcpEndpoint endpoint_;
        Pipeline::LocalPipeline &pipeline_;
    };
}

#endif //ASTERKV_NETWORK_TCP_SERVER_H
