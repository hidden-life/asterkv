#ifndef ASTERKV_NETWORK_TCP_CLIENT_H
#define ASTERKV_NETWORK_TCP_CLIENT_H

#include <cstdint>
#include <string>
#include <string_view>

#include <asterkv/core/result.h>
#include <asterkv/network/tcp_endpoint.h>

namespace AsterKV::Network {
    constexpr std::uint32_t defaultTcpClientIOTimeoutSeconds = 5;
    constexpr std::size_t defaultTcpClientMaxResponseBytes = 1024 * 1024;

    struct TcpLineClientOptions final {
        std::uint32_t ioTimeoutSeconds = defaultTcpClientIOTimeoutSeconds;
        std::size_t maxResponseBytes = defaultTcpClientMaxResponseBytes;
    };

    class TcpLineClient final {
    public:
        explicit TcpLineClient(TcpEndpoint endpoint, TcpLineClientOptions options = {});

        [[nodiscard]] const TcpEndpoint &endpoint() const noexcept;
        [[nodiscard]] const TcpLineClientOptions &options() const noexcept;

        [[nodiscard]] Core::Result<std::string> sendCommandLine(std::string_view commandLine) const;

    private:
        TcpEndpoint endpoint_;
        TcpLineClientOptions options_;
    };
}

#endif //ASTERKV_NETWORK_TCP_CLIENT_H
