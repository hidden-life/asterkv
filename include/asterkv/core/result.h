#ifndef ASTERKV_CORE_RESULT_H
#define ASTERKV_CORE_RESULT_H

#include <asterkv/core/status.h>

#include <type_traits>
#include <utility>
#include <variant>

namespace AsterKV::Core {
    template <typename T>
    class Result final {
        static_assert(!std::is_void_v<T>, "Use Status for operations without value.");
        static_assert(!std::is_same_v<std::remove_cvref_t<T>, Status>, "Use Status directly instead of Result<Status>.");

    public:
        [[nodiscard]] static Result success(T value) {
            return Result {std::move(value)};
        }

        [[nodiscard]] static Result failure(Status status) {
            return Result {normalizeFailureStatus(std::move(status))};
        }

        [[nodiscard]] bool isOk() const noexcept {
            return std::holds_alternative<T>(storage_);
        }

        [[nodiscard]] bool isError() const noexcept {
            return !isOk();
        }

        [[nodiscard]] Status status() const {
            if (isOk()) {
                return Status::ok();
            }

            return std::get<Status>(storage_);
        }

        [[nodiscard]] const T *valuePtr() const noexcept {
            return std::get_if<T>(&storage_);
        }

        [[nodiscard]] T *valuePtr() noexcept {
            return std::get_if<T>(&storage_);
        }

        [[nodiscard]] const T &value() const& {
            return std::get<T>(storage_);
        }

        [[nodiscard]] T &value() & {
            return std::get<T>(storage_);
        }

        [[nodiscard]] T &&value() && {
            return std::get<T>(std::move(storage_));
        }

    private:
        explicit Result(T value) : storage_(std::move(value)) {}

        explicit Result(Status status) : storage_(std::move(status)) {}

        [[nodiscard]] static Status normalizeFailureStatus(Status status) {
            if (status.isOk()) {
                return Status::internalError("Result failure cannot be constructed from OK status.");
            }

            return status;
        }

        std::variant<T, Status> storage_;
    };
}

#endif //ASTERKV_CORE_RESULT_H
