#include <asterkv/server/tcp_server_options.h>

#include "asterkv/network/tcp_endpoint.h"

namespace AsterKV::Server {
    TcpServerOptions defaultServerOptions() {
        return TcpServerOptions {
            .endpoint = Network::TcpEndpoint {
                .host = "127.0.0.1",
                .port = Network::defaultClientPort,
            },
            .maxClientWorkers = Network::defaultMaxClientWorkers,
            .clientIdleTimeoutSeconds = Network::defaultClientIdleTimeoutSeconds,
        };
    }
}
