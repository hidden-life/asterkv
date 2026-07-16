#include <asterkv/core/status.h>
#include <utility>

namespace AsterKV::Core {
    std::string_view errorCodeToString(ErrorCode code) noexcept {
        switch (code) {
            case ErrorCode::Ok:
                return "ok";
            case ErrorCode::Unknown:
                return "unknown";
            case ErrorCode::InvalidArgument:
                return "invalid_argument";
            case ErrorCode::NotFound:
                return "not_found";
            case ErrorCode::AlreadyExists:
                return "already_exists";
            case ErrorCode::Conflict:
                return "conflict";
            case ErrorCode::Unavailable:
                return "unavailable";
            case ErrorCode::Internal:
                return "internal";
        }

        return "unknown";
    }

    Status::Status(ErrorCode code, std::string message): errorCode_(code), message_(std::move(message)) {
    }

    Status Status::ok() {
        return Status {};
    }

    Status Status::unknown(std::string message) {
        return Status {ErrorCode::Unknown, std::move(message)};
    }

    Status Status::invalidArgument(std::string message) {
        return Status {ErrorCode::InvalidArgument, std::move(message)};
    }

    Status Status::notFound(std::string message) {
        return Status {ErrorCode::NotFound, std::move(message)};
    }

    Status Status::alreadyExists(std::string message) {
        return Status {ErrorCode::AlreadyExists, std::move(message)};
    }

    Status Status::conflict(std::string message) {
        return Status {ErrorCode::Conflict, std::move(message)};
    }

    Status Status::unavailable(std::string message) {
        return Status {ErrorCode::Unavailable, std::move(message)};
    }

    Status Status::internalError(std::string message) {
        return Status {ErrorCode::Internal, std::move(message)};
    }

    bool Status::isOk() const noexcept {
        return errorCode_ == ErrorCode::Ok;
    }

    ErrorCode Status::code() const noexcept {
        return errorCode_;
    }

    std::string_view Status::codeString() const noexcept {
        return errorCodeToString(errorCode_);
    }

    const std::string &Status::message() const noexcept {
        return message_;
    }
}
