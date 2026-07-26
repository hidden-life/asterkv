#ifndef ASTERKV_SERVER_TCP_SERVER_OPTIONS_H
#define ASTERKV_SERVER_TCP_SERVER_OPTIONS_H

#include <asterkv/network/tcp_endpoint.h>

namespace AsterKV::Server {
    struct TcpServerOptions final {
        Network::TcpEndpoint endpoint;
    };

    [[nodiscard]] TcpServerOptions defaultServerOptions();
}

#endif //ASTERKV_SERVER_TCP_SERVER_OPTIONS_H
