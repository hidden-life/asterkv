#include <asterkv/network/tcp_endpoint.h>

#include <cstdint>
#include <string>

namespace {

[[nodiscard]] bool expectEqual(AsterKV::Core::ErrorCode left,
                               AsterKV::Core::ErrorCode right) {
    return left == right;
}

[[nodiscard]] bool expectEqual(std::string_view left, std::string_view right) {
    return left == right;
}

[[nodiscard]] bool expectEqual(std::uint16_t left, std::uint16_t right) {
    return left == right;
}

[[nodiscard]] bool testParsesEndpoint() {
    const auto result = AsterKV::Network::parseTcpEndpoint("127.0.0.1:7721");

    if (!result.isOk()) {
        return false;
    }

    const AsterKV::Network::TcpEndpoint& endpoint = result.value();

    return expectEqual(endpoint.host, "127.0.0.1") &&
           expectEqual(endpoint.port, std::uint16_t{7721});
}

[[nodiscard]] bool testSerializesEndpoint() {
    const AsterKV::Network::TcpEndpoint endpoint{
        .host = "127.0.0.1",
        .port = std::uint16_t{7721},
    };

    return expectEqual(AsterKV::Network::tcpEndpointToString(endpoint), "127.0.0.1:7721");
}

[[nodiscard]] bool testRejectsMissingPort() {
    const auto result = AsterKV::Network::parseTcpEndpoint("127.0.0.1:");

    if (result.isOk()) {
        return false;
    }

    return expectEqual(result.status().code(), AsterKV::Core::ErrorCode::InvalidArgument);
}

[[nodiscard]] bool testRejectsMissingHost() {
    const auto result = AsterKV::Network::parseTcpEndpoint(":7721");

    if (result.isOk()) {
        return false;
    }

    return expectEqual(result.status().code(), AsterKV::Core::ErrorCode::InvalidArgument);
}

[[nodiscard]] bool testRejectsNonNumericPort() {
    const auto result = AsterKV::Network::parseTcpEndpoint("127.0.0.1:abc");

    if (result.isOk()) {
        return false;
    }

    return expectEqual(result.status().code(), AsterKV::Core::ErrorCode::InvalidArgument);
}

[[nodiscard]] bool testRejectsZeroPort() {
    const auto result = AsterKV::Network::parseTcpEndpoint("127.0.0.1:0");

    if (result.isOk()) {
        return false;
    }

    return expectEqual(result.status().code(), AsterKV::Core::ErrorCode::InvalidArgument);
}

[[nodiscard]] bool testRejectsTooLargePort() {
    const auto result = AsterKV::Network::parseTcpEndpoint("127.0.0.1:70000");

    if (result.isOk()) {
        return false;
    }

    return expectEqual(result.status().code(), AsterKV::Core::ErrorCode::InvalidArgument);
}

[[nodiscard]] bool testRejectsMultipleSeparators() {
    const auto result = AsterKV::Network::parseTcpEndpoint("127.0.0.1:7721:extra");

    if (result.isOk()) {
        return false;
    }

    return expectEqual(result.status().code(), AsterKV::Core::ErrorCode::InvalidArgument);
}

} // namespace

int main() {
    if (!testParsesEndpoint()) {
        return 1;
    }

    if (!testSerializesEndpoint()) {
        return 1;
    }

    if (!testRejectsMissingPort()) {
        return 1;
    }

    if (!testRejectsMissingHost()) {
        return 1;
    }

    if (!testRejectsNonNumericPort()) {
        return 1;
    }

    if (!testRejectsZeroPort()) {
        return 1;
    }

    if (!testRejectsTooLargePort()) {
        return 1;
    }

    if (!testRejectsMultipleSeparators()) {
        return 1;
    }

    return 0;
}
