#ifndef ASTERKV_CORE_STATUS_H
#define ASTERKV_CORE_STATUS_H

#include <string_view>
#include <string>

namespace AsterKV::Core {
    enum class ErrorCode {
        Ok = 0,
        Unknown,
        InvalidArgument,
        NotFound,
        AlreadyExists,
        Conflict,
        Unavailable,
        Internal,
    };

    [[nodiscard]] std::string_view errorCodeToString(ErrorCode code) noexcept;

    class Status final {
    public:
        Status() = default;
        Status(ErrorCode code, std::string message);

        [[nodiscard]] static Status ok();
        [[nodiscard]] static Status unknown(std::string message);
        [[nodiscard]] static Status invalidArgument(std::string message);
        [[nodiscard]] static Status notFound(std::string message);
        [[nodiscard]] static Status alreadyExists(std::string message);
        [[nodiscard]] static Status conflict(std::string message);
        [[nodiscard]] static Status unavailable(std::string message);
        [[nodiscard]] static Status internalError(std::string message);

        [[nodiscard]] bool isOk() const noexcept;
        [[nodiscard]] ErrorCode code() const noexcept;
        [[nodiscard]] std::string_view codeString() const noexcept;
        [[nodiscard]] const std::string &message() const noexcept;

    private:
        ErrorCode errorCode_ = ErrorCode::Ok;
        std::string message_;
    };
}

#endif //ASTERKV_CORE_STATUS_H
