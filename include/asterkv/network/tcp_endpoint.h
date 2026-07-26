#ifndef ASTERKV_NETWORK_TCP_ENDPOINT_H
#define ASTERKV_NETWORK_TCP_ENDPOINT_H

#include <cstdint>
#include <string>

#include <asterkv/core/result.h>

namespace AsterKV::Network {
    constexpr std::uint16_t defaultClientPort = 7721;

    struct TcpEndpoint final {
        std::string host;
        std::uint16_t port;
    };

    [[nodiscard]] Core::Result<TcpEndpoint> parseTcpEndpoint(std::string_view value);
    [[nodiscard]] std::string tcpEndpointToString(const TcpEndpoint &endpoint);
}

#endif //ASTERKV_NETWORK_TCP_ENDPOINT_H
