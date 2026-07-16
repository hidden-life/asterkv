#ifndef ASTERKV_EXECUTION_COMMAND_DISPATCHER_H
#define ASTERKV_EXECUTION_COMMAND_DISPATCHER_H

#include <asterkv/storage/in_memory_storage.h>
#include <asterkv/execution/command_response.h>
#include <asterkv/command/command.h>

namespace AsterKV::Execution {
    class CommandDispatcher final {
    public:
        explicit CommandDispatcher(Storage::StorageEngine &storage);
        [[nodiscard]] Core::Result<CommandResponse> dispatch(const Command::CommandRequest &request);

    private:
        Storage::StorageEngine &storage_;
    };
}

#endif //ASTERKV_EXECUTION_COMMAND_DISPATCHER_H
