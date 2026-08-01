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
#include <asterkv/config/server_config.h>
#include <asterkv/logging/logger.h>
#include <asterkv/network/tcp_client.h>
#include <asterkv/client/response_renderer.h>
#include <asterkv/wal/wal_record.h>
#include <asterkv/wal/wal_record_codec.h>
#include <asterkv/wal/wal_file_writer.h>

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

    if (AsterKV::Logging::logLevelToString(AsterKV::Logging::LogLevel::Info) != "info") {
        return 1;
    }

    auto parsedLogLevel = AsterKV::Logging::logLevelFromString("warn");
    if (parsedLogLevel.isError()) {
        return 1;
    }

    if (parsedLogLevel.value() != AsterKV::Logging::LogLevel::Warn) {
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

    if (options.logLevel != AsterKV::Logging::defaultLogLevel) {
        return 1;
    }

    if (options.clientIdleTimeoutSeconds != AsterKV::Network::defaultClientIdleTimeoutSeconds) {
        return 1;
    }

    if (options.maxClientWorkers != AsterKV::Network::defaultMaxClientWorkers) {
        return 1;
    }

    auto parsedConfig = AsterKV::Config::parseServerConfig(
        "listen = 127.0.0.1:7721\n"
        "max_clients = 128\n"
        "idle_timeout_seconds = 300\n"
        "log_level = warn\n"
    );

    if (parsedConfig.isError()) {
        return 1;
    }

    if (parsedConfig.value().logLevel != AsterKV::Logging::LogLevel::Warn) {
        return 1;
    }

    if (parsedConfig.value().endpoint.port != AsterKV::Network::defaultClientPort) {
        return 1;
    }

    const AsterKV::Wal::WalRecord publicWalRecord =
    AsterKV::Wal::makeSetRecord(1, "public_header_key", "public_header_value");

    if (!AsterKV::Wal::validateWalRecord(publicWalRecord).isOk()) {
        return 1;
    }

    if (AsterKV::Wal::walRecordTypeToString(publicWalRecord.type) != "set") {
        return 1;
    }

    const auto serializedPublicWalRecord =
        AsterKV::Wal::serialize(publicWalRecord);

    if (serializedPublicWalRecord.isError()) {
        return 1;
    }

    const auto deserializedPublicWalRecord =
        AsterKV::Wal::deserialize(serializedPublicWalRecord.value());

    if (deserializedPublicWalRecord.isError()) {
        return 1;
    }

    if (deserializedPublicWalRecord.value().key != publicWalRecord.key) {
        return 1;
    }

    if (deserializedPublicWalRecord.value().value != publicWalRecord.value) {
        return 1;
    }

    const AsterKV::Wal::WalFileWriterOptions publicWalWriterOptions{};

    if (!publicWalWriterOptions.flushAfterWrite) {
        return 1;
    }

    const AsterKV::Wal::WalFileWriter publicWalWriter{
        "public_headers_test.wal",
        publicWalWriterOptions,
    };

    if (publicWalWriter.path().empty()) {
        return 1;
    }

    if (!publicWalWriter.options().flushAfterWrite) {
        return 1;
    }

    AsterKV::Network::TcpLineClientOptions clientOptions {};
    if (clientOptions.ioTimeoutSeconds != AsterKV::Network::defaultTcpClientIOTimeoutSeconds) {
        return 1;
    }

    if (clientOptions.maxResponseBytes != AsterKV::Network::defaultTcpClientMaxResponseBytes) {
        return 1;
    }

    auto renderedClientResponse = AsterKV::Client::renderPrettyResponseText("+OK\n");

    if (renderedClientResponse.isError()) {
        return 1;
    }

    if (renderedClientResponse.value() != "OK\n") {
        return 1;
    }

    AsterKV::Server::SignalShutdownController::reset();
    if (AsterKV::Server::SignalShutdownController::stopRequested()) {
        return 1;
    }

    AsterKV::Server::TcpServerRuntime runtime {options};

    return runtime.endpoint().port == AsterKV::Network::defaultClientPort ? 0 : 1;
}
