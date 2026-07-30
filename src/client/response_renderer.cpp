#include <limits>
#include <asterkv/client/response_renderer.h>

namespace AsterKV::Client {
    namespace {
        struct FirstLine final {
            std::string_view line;
            std::size_t nextIndex;
        };

        [[nodiscard]] Core::Result<FirstLine> readFirstLine(std::string_view input) {
            if (input.empty()) {
                return Core::Result<FirstLine>::failure(
                    Core::Status::invalidArgument("protocol response is empty")
                );
            }

            const std::size_t newLineIndex = input.find('\n');
            if (newLineIndex == std::string::npos) {
                std::string_view line = input;

                if (!line.empty() && line.back() == '\r') {
                    line.remove_suffix(1);
                }

                return Core::Result<FirstLine>::success(FirstLine {
                    .line = line,
                    .nextIndex = input.size()
                });
            }

            std::string_view line = input.substr(0, newLineIndex);
            if (!line.empty() && line.back() == '\r') {
                line.remove_suffix(1);
            }

            return Core::Result<FirstLine>::success(FirstLine {
                .line = line,
                .nextIndex = newLineIndex + 1
            });
        }

        [[nodiscard]] bool isDigit(char ch) noexcept {
            return ch >= '0' && ch <= '9';
        }

        [[nodiscard]] Core::Result<std::size_t> parseBulkLength(std::string_view value) {
            if (value.empty()) {
                return Core::Result<std::size_t>::failure(Core::Status::invalidArgument("bulk string length is empty"));
            }

            std::size_t result = 0;

            for (const char ch : value) {
                if (!isDigit(ch)) {
                    return Core::Result<std::size_t>::failure(Core::Status::invalidArgument("bulk string length is not numeric"));
                }

                const std::size_t digit = static_cast<std::size_t>(ch - '0');

                if (result > (std::numeric_limits<std::size_t>::max() - digit) / 10) {
                    return Core::Result<std::size_t>::failure(Core::Status::invalidArgument("bulk string length is too large"));
                }

                result = result * 10 + digit;
            }

            return Core::Result<std::size_t>::success(result);
        }

        [[nodiscard]] bool isValidIntegerPayload(std::string_view value) noexcept {
            if (value.empty()) {
                return false;
            }

            std::size_t index = 0;

            if (value.front() == '-') {
                if (value.size() == 1) {
                    return false;
                }

                index = 1;
            }

            for (; index < value.size(); ++index) {
                if (!isDigit(value[index])) {
                    return false;
                }
            }

            return true;
        }

        [[nodiscard]] std::string stripErrorPrefix(std::string_view value) {
            constexpr std::string_view errPrefix = "ERR ";

            if (value.size() >= errPrefix.size() && value.substr(0, errPrefix.size()) == errPrefix) {
                value.remove_prefix(errPrefix.size());
            }

            return std::string {value};
        }

        [[nodiscard]] bool hasValidBulkTrailingBytes(std::string_view bytes) noexcept {
            return bytes.empty() || bytes == "\n" || bytes == "\r\n";
        }

        [[nodiscard]] Core::Result<ProtocolResponse> parseLineResponse(ProtocolResponseKind kind, std::string_view raw) {
            Core::Result<FirstLine> firstLine = readFirstLine(raw);

            if (firstLine.isError()) {
                return Core::Result<ProtocolResponse>::failure(firstLine.status());
            }

            const std::string_view line = firstLine.value().line;

            if (line.size() < 2) {
                return Core::Result<ProtocolResponse>::failure(
                    Core::Status::invalidArgument("protocol response line is incomplete")
                );
            }

            std::string_view value = line.substr(1);

            if (kind == ProtocolResponseKind::Integer && !isValidIntegerPayload(value)) {
                return Core::Result<ProtocolResponse>::failure(
                    Core::Status::invalidArgument("integer response payload is invalid")
                );
            }

            return Core::Result<ProtocolResponse>::success(
                ProtocolResponse {
                    .kind = kind,
                    .value = std::string {value}
                }
            );
        }

        [[nodiscard]] Core::Result<ProtocolResponse> parseBulkStringResponse(std::string_view raw) {
            Core::Result<FirstLine> firstLine = readFirstLine(raw);
            if (firstLine.isError()) {
                return Core::Result<ProtocolResponse>::failure(firstLine.status());
            }

            const std::string_view line = firstLine.value().line;

            if (line.size() < 2) {
                return Core::Result<ProtocolResponse>::failure(
                    Core::Status::invalidArgument("bulk string response line is incomplete")
                );
            }

            Core::Result<std::size_t> parsed = parseBulkLength(line.substr(1));
            if (parsed.isError()) {
                return Core::Result<ProtocolResponse>::failure(parsed.status());
            }

            const std::size_t length = parsed.value();

            if (firstLine.value().nextIndex > raw.size()) {
                return Core::Result<ProtocolResponse>::failure(
                    Core::Status::invalidArgument("bulk string response payload is missing")
                );
            }

            const std::string_view remaining = raw.substr(firstLine.value().nextIndex);

            if (remaining.size() < length) {
                return Core::Result<ProtocolResponse>::failure(
                    Core::Status::invalidArgument("bulk string response payload is truncated")
                );
            }

            const std::string_view payload = remaining.substr(0, length);
            const std::string_view trailingBytes = remaining.substr(length);

            if (!hasValidBulkTrailingBytes(trailingBytes)) {
                return Core::Result<ProtocolResponse>::failure(
                    Core::Status::invalidArgument("bulk string response has invalid trailing bytes")
                );
            }

            return Core::Result<ProtocolResponse>::success(
                ProtocolResponse {
                    .kind = ProtocolResponseKind::BulkString,
                    .value = std::string {payload}
                }
            );
        }
    }

    Core::Result<ProtocolResponse> parseProtocolResponse(std::string_view raw) {
        if (raw.empty()) {
            return Core::Result<ProtocolResponse>::failure(
                Core::Status::invalidArgument("protocol response is empty")
            );
        }

        switch (raw.front()) {
            case '+':
                return parseLineResponse(ProtocolResponseKind::SimpleString, raw);

            case '-':
                return parseLineResponse(ProtocolResponseKind::Error, raw);

            case ':':
                return parseLineResponse(ProtocolResponseKind::Integer, raw);

            case '$':
                return parseBulkStringResponse(raw);

            default:
                return Core::Result<ProtocolResponse>::failure(
                    Core::Status::invalidArgument("unknown protocol response prefix")
                );
        }
    }

    std::string renderPrettyResponse(const ProtocolResponse &response) {
        switch (response.kind) {
            case ProtocolResponseKind::BulkString:
            case ProtocolResponseKind::SimpleString:
            case ProtocolResponseKind::Integer: {
                std::string out = response.value;
                out.push_back('\n');

                return out;
            }
            case ProtocolResponseKind::Error: {
                std::string out = "error: ";
                out.append(stripErrorPrefix(response.value));
                out.push_back('\n');

                return out;
            }
        }

        return "error: unknown response kind\n";
    }

    Core::Result<std::string> renderPrettyResponseText(std::string_view raw) {
        Core::Result<ProtocolResponse> response = parseProtocolResponse(raw);

        if (response.isError()) {
            return Core::Result<std::string>::failure(response.status());
        }

        return Core::Result<std::string>::success(renderPrettyResponse(response.value()));
    }
}
