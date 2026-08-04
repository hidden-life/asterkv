#ifndef ASTERKV_SERVER_TCP_SERVER_RUNTIME_H
#define ASTERKV_SERVER_TCP_SERVER_RUNTIME_H

#include <asterkv/core/status.h>
#include <asterkv/network/tcp_endpoint.h>
#include <asterkv/network/tcp_server.h>
#include <asterkv/pipeline/local_pipeline.h>
#include <asterkv/server/tcp_server_options.h>
#include <asterkv/storage/in_memory_storage.h>
#include <asterkv/wal/wal_backed_storage.h>

#include <memory>

namespace AsterKV::Server {
    class TcpServerRuntime final {
    public:
        explicit TcpServerRuntime(TcpServerOptions options);

        [[nodiscard]] const TcpServerOptions &options() const noexcept;
        [[nodiscard]] const Network::TcpEndpoint &endpoint() const noexcept;

        [[nodiscard]] Core::Status run();

    private:
        TcpServerOptions options_;
        Storage::InMemoryStorage storage_;
        std::unique_ptr<Wal::WalBackedStorage> walStorage_;
        Pipeline::LocalPipeline pipeline_;
        Network::TcpLineServer server_;
    };
}

#endif //ASTERKV_SERVER_TCP_SERVER_RUNTIME_H
