#include <asterkv/execution/command_dispatcher.h>

namespace AsterKV::Execution {
    namespace {
        [[nodiscard]] Core::Result<CommandResponse> dispatchPing() {
            return Core::Result<CommandResponse>::success(
                CommandResponse::simpleString("PONG")
            );
        }

        [[nodiscard]] Core::Result<CommandResponse> dispatchSet(
            Storage::StorageEngine &storage,
            const Command::CommandRequest &request
        ) {
            Core::Status status = storage.set(request.arguments[0], request.arguments[1]);
            if (!status.isOk()) {
                return Core::Result<CommandResponse>::failure(std::move(status));
            }

            return Core::Result<CommandResponse>::success(CommandResponse::simpleString("OK"));
        }

        [[nodiscard]] Core::Result<CommandResponse> dispatchGet(
            const Storage::StorageEngine &storage,
            const Command::CommandRequest &request
        ) {
            Core::Result<std::string> value = storage.get(request.arguments[0]);
            if (value.isError()) {
                return Core::Result<CommandResponse>::failure(value.status());
            }

            return Core::Result<CommandResponse>::success(CommandResponse::bulkString(value.value()));
        }

        [[nodiscard]] Core::Result<CommandResponse> dispatchDel(
            Storage::StorageEngine &storage,
            const Command::CommandRequest &request
        ) {
            Core::Status status = storage.remove(request.arguments[0]);
            if (status.isOk()) {
                return Core::Result<CommandResponse>::success(CommandResponse::integer(std::int64_t { 1 }));
            }

            if (status.code() == Core::ErrorCode::NotFound) {
                return Core::Result<CommandResponse>::success(CommandResponse::integer(std::int64_t { 0 }));
            }

            return Core::Result<CommandResponse>::failure(std::move(status));
        }

        [[nodiscard]] Core::Result<CommandResponse> dispatchExists(
            const Storage::StorageEngine &storage,
            const Command::CommandRequest &request
        ) {
            Core::Result<bool> exists = storage.exists(request.arguments[0]);
            if (exists.isError()) {
                return Core::Result<CommandResponse>::failure(exists.status());
            }

            return Core::Result<CommandResponse>::success(
                CommandResponse::integer(exists.value() ? std::int64_t { 1 } : std::int64_t { 0 })
            );
        }
    }

    CommandDispatcher::CommandDispatcher(Storage::StorageEngine &storage) : storage_(storage) {
    }

    Core::Result<CommandResponse> CommandDispatcher::dispatch(const Command::CommandRequest &request) {
        Core::Status validationStatus = Command::validateCommandRequest(request);
        if (!validationStatus.isOk()) {
            return Core::Result<CommandResponse>::failure(std::move(validationStatus));
        }

        switch (request.type) {
            case Command::CommandType::Ping:
                return dispatchPing();
            case Command::CommandType::Set:
                return dispatchSet(storage_, request);
            case Command::CommandType::Get:
                return dispatchGet(storage_, request);
            case Command::CommandType::Del:
                return dispatchDel(storage_, request);
            case Command::CommandType::Exists:
                return dispatchExists(storage_, request);
        }

        return Core::Result<CommandResponse>::failure(Core::Status::internalError("unsupported command type"));
    }
}
