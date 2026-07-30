#ifndef ASTERKV_CLIENT_RESPONSE_RENDERER_H
#define ASTERKV_CLIENT_RESPONSE_RENDERER_H

#include <string>

#include <asterkv/core/result.h>

namespace AsterKV::Client {
    enum class ProtocolResponseKind {
        SimpleString,
        Error,
        Integer,
        BulkString,
    };

    struct ProtocolResponse final {
        ProtocolResponseKind kind;
        std::string value;
    };

    [[nodiscard]] Core::Result<ProtocolResponse> parseProtocolResponse(std::string_view raw);

    [[nodiscard]] std::string renderPrettyResponse(const ProtocolResponse &response);

    [[nodiscard]] Core::Result<std::string> renderPrettyResponseText(std::string_view raw);
}

#endif //ASTERKV_CLIENT_RESPONSE_RENDERER_H
