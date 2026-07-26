#ifndef ASTERKV_PROTOCOL_PARSER_H
#define ASTERKV_PROTOCOL_PARSER_H

#include <vector>
#include <string>

#include <asterkv/command/command.h>
#include <asterkv/core/result.h>

namespace AsterKV::Protocol {
    [[nodiscard]] std::vector<std::string> tokenizeCommandLine(std::string_view line);
    [[nodiscard]] Core::Result<Command::CommandRequest> parseCommandLine(std::string_view line);
}

#endif //ASTERKV_PROTOCOL_PARSER_H
