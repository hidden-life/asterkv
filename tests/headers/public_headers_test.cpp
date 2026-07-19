#include <asterkv/core/result.h>
#include <asterkv/core/status.h>
#include <asterkv/core/version.h>
#include <asterkv/command/command.h>
#include <asterkv/protocol/parser.h>
#include <asterkv/storage/in_memory_storage.h>
#include <asterkv/storage/storage_engine.h>
#include <asterkv/execution/command_dispatcher.h>
#include <asterkv/protocol/response_serializer.h>
#include <asterkv/pipeline/local_pipeline.h>

#include <string>

int main() {
    auto result = AsterKV::Core::Result<std::string>::success("value");
    if (!result.isOk()) {
        return 1;
    }

    const AsterKV::Core::Status status = AsterKV::Core::Status::ok();

    if (!status.isOk()) {
        return 1;
    }

    const AsterKV::Core::Version currentVersion = AsterKV::Core::version();
    if (currentVersion.major != 0) {
        return 1;
    }

    auto commandType = AsterKV::Command::commandTypeFromString("PING");
    if (!commandType.isOk()) {
        return 1;
    }

    auto commandRequest = AsterKV::Protocol::parseCommandLine("PING");
    if (!commandRequest.isOk()) {
        return 1;
    }

    AsterKV::Storage::InMemoryStorage storage;
    AsterKV::Execution::CommandDispatcher dispatcher{storage};
    auto cmdResponse = dispatcher.dispatch(commandRequest.value());
    if (!cmdResponse.isOk()) {
        return 1;
    }

    const std::string serialized = AsterKV::Protocol::serializeExecutionResult(cmdResponse);

    if (serialized != "+PONG\r\n") {
        return 1;
    }

    AsterKV::Pipeline::LocalPipeline pipeline{storage};

    return pipeline.processLine("PING") == "+PONG\r\n" ? 0 : 1;
}
