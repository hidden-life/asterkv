#ifndef ASTERKV_PROTOCOL_RESPONSE_SERIALIZER_H
#define ASTERKV_PROTOCOL_RESPONSE_SERIALIZER_H

#include <string>

#include <asterkv/execution/command_dispatcher.h>

namespace AsterKV::Protocol {
    [[nodiscard]] std::string serializeCommandResponse(const Execution::CommandResponse &response);
    [[nodiscard]] std::string serializeStatus(const Core::Status &status);
    [[nodiscard]] std::string serializeExecutionResult(const Core::Result<Execution::CommandResponse> &result);
}

#endif //ASTERKV_PROTOCOL_RESPONSE_SERIALIZER_H
