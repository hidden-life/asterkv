#include <charconv>
#include <asterkv/network/tcp_endpoint.h>

#include <asterkv/core/result.h>

namespace AsterKV::Network {
    namespace {
        constexpr std::uint32_t maxTcpPort = 65535;

        [[nodiscard]] Core::Status invalidEndpointFormat() {
            return Core::Status::invalidArgument("tcp endpoint must use host:port format");
        }

        [[nodiscard]] Core::Status invalidEndpointPort() {
            return Core::Status::invalidArgument("tcp endpoint must be in range 1..65535");
        }
    }

    Core::Result<TcpEndpoint> parseTcpEndpoint(std::string_view value) {
        const std::size_t separatorPosition = value.find(':');
        if (separatorPosition == std::string_view::npos || separatorPosition == 0 || separatorPosition + 1 >= value.size()) {
            return Core::Result<TcpEndpoint>::failure(invalidEndpointFormat());
        }

        if (value.find(':', separatorPosition + 1) != std::string_view::npos) {
            return Core::Result<TcpEndpoint>::failure(invalidEndpointFormat());
        }

        const std::string_view host = value.substr(0, separatorPosition);
        const std::string_view portText = value.substr(separatorPosition + 1);
        std::uint32_t parsedPort = 0;

        const char* const begin = portText.data();
        const char* const end = portText.data() + portText.size();
        const auto parseResult = std::from_chars(begin, end, parsedPort);

        if (parseResult.ec != std::errc {} || parseResult.ptr != end || parsedPort == 0 || parsedPort > maxTcpPort) {
            return Core::Result<TcpEndpoint>::failure(invalidEndpointPort());
        }

        return Core::Result<TcpEndpoint>::success(TcpEndpoint {
            .host = std::string {host},
            .port = static_cast<std::uint16_t>(parsedPort),
        });
    }

    std::string tcpEndpointToString(const TcpEndpoint &endpoint) {
        return endpoint.host + ":" + std::to_string(endpoint.port);
    }
}
