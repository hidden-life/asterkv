#ifndef ASTERKV_SERVER_TCP_SERVER_OPTIONS_H
#define ASTERKV_SERVER_TCP_SERVER_OPTIONS_H

#include <asterkv/network/tcp_endpoint.h>
#include <asterkv/network/tcp_server.h>

#include <cstddef>
#include <cstdint>

namespace AsterKV::Server {
    struct TcpServerOptions final {
        Network::TcpEndpoint endpoint;
        std::size_t maxClientWorkers;
        std::uint32_t clientIdleTimeoutSeconds;
    };

    [[nodiscard]] TcpServerOptions defaultServerOptions();
}

#endif //ASTERKV_SERVER_TCP_SERVER_OPTIONS_H
