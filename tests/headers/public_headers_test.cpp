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
#include <asterkv/network/tcp_endpoint.h>
#include <asterkv/network/tcp_server.h>
#include <asterkv/server/signal_shutdown_controller.h>
#include <asterkv/server/tcp_server_options.h>
#include <asterkv/server/tcp_server_runtime.h>

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

    if (pipeline.processLine("PING") != "+PONG\r\n") {
        return 1;
    }

    auto endpoint = AsterKV::Network::parseTcpEndpoint("127.0.0.1:7721");
    if (!endpoint.isOk()) {
        return 1;
    }

    AsterKV::Network::TcpLineServer server {endpoint.value(), pipeline};
    if (server.endpoint().port != AsterKV::Network::defaultClientPort) {
        return 1;
    }

    const AsterKV::Server::TcpServerOptions options = AsterKV::Server::defaultServerOptions();
    if (options.endpoint.port != AsterKV::Network::defaultClientPort) {
        return 1;
    }

    AsterKV::Server::SignalShutdownController::reset();
    if (AsterKV::Server::SignalShutdownController::stopRequested()) {
        return 1;
    }

    AsterKV::Server::TcpServerRuntime runtime {options};

    return runtime.endpoint().port == AsterKV::Network::defaultClientPort ? 0 : 1;
}
