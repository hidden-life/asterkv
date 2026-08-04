#ifndef ASTERKV_SERVER_TCP_SERVER_OPTIONS_H
#define ASTERKV_SERVER_TCP_SERVER_OPTIONS_H

#include <asterkv/network/tcp_endpoint.h>
#include <asterkv/network/tcp_server.h>
#include <asterkv/logging/logger.h>

#include <cstddef>
#include <string>

namespace AsterKV::Server {
    struct TcpServerOptions final {
        Network::TcpEndpoint endpoint;
        std::size_t maxClientWorkers;
        std::uint32_t clientIdleTimeoutSeconds;
        Logging::LogLevel logLevel;
        std::string walFilePath;
    };

    [[nodiscard]] TcpServerOptions defaultServerOptions();
}

#endif //ASTERKV_SERVER_TCP_SERVER_OPTIONS_H
