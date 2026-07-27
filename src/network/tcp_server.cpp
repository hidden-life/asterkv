#include <unistd.h>
#include <string>
#include <cerrno>
#include <utility>
#include <cstring>
#include <array>
#include <memory>
#include <thread>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <asterkv/network/tcp_server.h>
#include <asterkv/core/result.h>
#include <asterkv/pipeline/local_pipeline.h>
#include <asterkv/network/tcp_endpoint.h>

namespace AsterKV::Network {
    namespace {
        constexpr std::string_view clientLimitResponse = "-ERR unavailable maximum client worker limit reached\r\n";

        class UniqueFd final {
        public:
            explicit UniqueFd(int fd) noexcept : fd_(fd) {}

            ~UniqueFd() {
                reset();
            }

            UniqueFd(const UniqueFd&) = delete;
            UniqueFd& operator=(const UniqueFd&) = delete;

            UniqueFd(UniqueFd&& other) noexcept : fd_(other.release()) {}

            UniqueFd& operator=(UniqueFd&& other) noexcept {
                if (this != &other) {
                    reset(other.release());
                }

                return *this;
            }

            [[nodiscard]] int get() const noexcept {
                return fd_;
            }

            [[nodiscard]] int release() noexcept {
                const int fd = fd_;
                fd_ = -1;

                return fd;
            }

            void reset(int fd = -1) noexcept {
                if (fd_ >= 0) {
                    static_cast<void>(::close(fd_));
                }

                fd_ = fd;
            }

        private:
            int fd_;
        };

        [[nodiscard]] bool isStopRequested(TcpLineServer::StopRequestedCallback stopRequested) noexcept {
            return stopRequested != nullptr && stopRequested();
        }

        enum class AcceptClientResultType {
            Accepted,
            Stopped,
            Failed,
        };

        struct AcceptClientResult final {
            AcceptClientResultType type;
            UniqueFd clientFd;
            Core::Status status;

            [[nodiscard]] static AcceptClientResult accepted(UniqueFd clientFd) {
                return AcceptClientResult {
                    .type = AcceptClientResultType::Accepted,
                    .clientFd = std::move(clientFd),
                    .status = Core::Status::ok(),
                };
            }

            [[nodiscard]] static AcceptClientResult stopped() {
                return AcceptClientResult {
                    .type = AcceptClientResultType::Stopped,
                    .clientFd = UniqueFd {-1},
                    .status = Core::Status::ok(),
                };
            }

            [[nodiscard]] static AcceptClientResult failed(Core::Status status) {
                return AcceptClientResult {
                    .type = AcceptClientResultType::Failed,
                    .clientFd = UniqueFd {-1},
                    .status = std::move(status),
                };
            }
        };

        struct ClientWorkerSlot final {
            std::thread thread;
            std::shared_ptr<std::atomic_bool> finished;
        };

        [[nodiscard]] Core::Status validateOptions(const TcpLineServerOptions &options) {
            if (options.maxClientWorkers == 0) {
                return Core::Status::invalidArgument("max client workers must be greater than zero");
            }

            return Core::Status::ok();
        }

        [[nodiscard]] Core::Status systemUnavailableError(std::string_view context) {
            std::string message {context};
            message.append(": ");
            message.append(std::strerror(errno));

            return Core::Status::unavailable(std::move(message));
        }

        [[nodiscard]] Core::Status configureClientSocket(int clientFd) {
            timeval timeout {};
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;

            if (::setsockopt(clientFd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
                return systemUnavailableError("failed to configure client receive timeout");
            }

            if (::setsockopt(clientFd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
                return systemUnavailableError("failed to configure client send timeout");
            }

            return Core::Status::ok();
        }

        [[nodiscard]] bool isTemporarySocketWaitError() noexcept {
            return errno == EAGAIN || errno == EWOULDBLOCK;
        }

        [[nodiscard]] Core::Status sendAll(int clientFd, std::string_view bytes, TcpLineServer::StopRequestedCallback stopRequested) {
            std::size_t totalSent = 0;

            while (totalSent < bytes.size()) {
                const ssize_t sentCount = ::send(
                    clientFd,
                    bytes.data() + totalSent,
                    bytes.size() - totalSent,
                    MSG_NOSIGNAL
                );

                if (sentCount < 0) {
                    if (errno == EINTR || isTemporarySocketWaitError()) {
                        if (isStopRequested(stopRequested)) {
                            return Core::Status::ok();
                        }

                        continue;
                    }

                    return systemUnavailableError("failed to write client connection");
                }

                if (sentCount == 0) {
                    return Core::Status::unavailable("failed to write client connection: zero bytes written");
                }

                totalSent += static_cast<std::size_t>(sentCount);
            }

            return Core::Status::ok();
        }

        [[nodiscard]] Core::Status processLineAndSendResponse(
            int clientFd,
            Pipeline::LocalPipeline &pipeline,
            std::string_view rawLine,
            TcpLineServer::StopRequestedCallback stopRequested
            ) {
            std::string_view line = rawLine;

            if (!line.empty() && line.back() == '\r') {
                line.remove_suffix(1);
            }

            const std::string response = pipeline.processLine(line);

            return sendAll(clientFd, response, stopRequested);
        }

        [[nodiscard]] Core::Status processBufferedLines(
            int clientFd,
            Pipeline::LocalPipeline &pipeline,
            std::string &input,
            TcpLineServer::StopRequestedCallback stopRequested
            ) {
            std::size_t newlinePosition = input.find('\n');
            while (newlinePosition != std::string::npos) {
                const std::string line = input.substr(0, newlinePosition);
                Core::Status status = processLineAndSendResponse(clientFd, pipeline, line, stopRequested);
                if (!status.isOk()) {
                    return status;
                }

                input.erase(0, newlinePosition + 1);
                newlinePosition = input.find('\n');
            }

            return Core::Status::ok();
        }

        [[nodiscard]] Core::Status serveClient(
            int clientFd,
            Pipeline::LocalPipeline &pipeline,
            TcpLineServer::StopRequestedCallback stopRequested
            ) {
            std::array<char, 4096> buffer {};
            std::string pendingInput;

            while (!isStopRequested(stopRequested)) {
                const ssize_t readCount = ::recv(clientFd, buffer.data(), buffer.size(), 0);
                if (readCount < 0) {
                    if (errno == EINTR || isTemporarySocketWaitError()) {
                        if (isStopRequested(stopRequested)) {
                            return Core::Status::ok();
                        }

                        continue;
                    }

                    return systemUnavailableError("failed to read client connection");
                }

                if (readCount == 0) {
                    break;
                }

                pendingInput.append(buffer.data(), static_cast<std::size_t>(readCount));
                Core::Status status = processBufferedLines(clientFd, pipeline, pendingInput, stopRequested);

                if (!status.isOk()) {
                    return status;
                }
            }

            if (!pendingInput.empty() && !isStopRequested(stopRequested)) {
                return processLineAndSendResponse(clientFd, pipeline, pendingInput, stopRequested);
            }

            return Core::Status::ok();
        }

        [[nodiscard]] Core::Result<sockaddr_in> makeSocketAddress(const TcpEndpoint &endpoint) {
            sockaddr_in address {};
            address.sin_family = AF_INET;
            address.sin_port = htons(endpoint.port);

            const int result = ::inet_pton(AF_INET, endpoint.host.c_str(), &address.sin_addr);

            if (result != 1) {
                return Core::Result<sockaddr_in>::failure(
                    Core::Status::invalidArgument("listen host must be a valid IPv4 address")
                );
            }

            return Core::Result<sockaddr_in>::success(address);
        }

        [[nodiscard]] Core::Result<UniqueFd> createListeningSocket(const TcpEndpoint &endpoint) {
            Core::Result<sockaddr_in> sockerAddr = makeSocketAddress(endpoint);

            if (sockerAddr.isError()) {
                return Core::Result<UniqueFd>::failure(sockerAddr.status());
            }

            const int rawServerFd = ::socket(AF_INET, SOCK_STREAM, 0);
            if (rawServerFd < 0) {
                return Core::Result<UniqueFd>::failure(systemUnavailableError("failed to create server socket"));
            }

            UniqueFd serverFd {rawServerFd};
            int reuseAddress = 1;
            if (::setsockopt(serverFd.get(), SOL_SOCKET, SO_REUSEADDR, &reuseAddress, sizeof(reuseAddress)) < 0) {
                return Core::Result<UniqueFd>::failure(systemUnavailableError("failed to configure server socket"));
            }

            sockaddr_in addr = sockerAddr.value();
            if (::bind(serverFd.get(), reinterpret_cast<const sockaddr*>(&addr), sizeof(addr)) < 0) {
                return Core::Result<UniqueFd>::failure(systemUnavailableError("failed to bind server socket"));
            }

            constexpr int backlog = 16;
            if (::listen(serverFd.get(), backlog) < 0) {
                return Core::Result<UniqueFd>::failure(systemUnavailableError("failed to listen on server socket"));
            }

            return Core::Result<UniqueFd>::success(std::move(serverFd));
        }

        [[nodiscard]] AcceptClientResult acceptClient(int serverFd, TcpLineServer::StopRequestedCallback stopRequested) {
            while (!isStopRequested(stopRequested)) {
                sockaddr_in clientAddr {};
                socklen_t clientAddrLen = sizeof(clientAddr);
                const int rawClientFd = ::accept(serverFd, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrLen);
                if (rawClientFd < 0) {
                    if (errno == EINTR) {
                        if (isStopRequested(stopRequested)) {
                            return AcceptClientResult::stopped();
                        }

                        continue;
                    }

                    return AcceptClientResult::failed(systemUnavailableError("failed to accept client connection"));
                }

                UniqueFd clientFd {rawClientFd};
                Core::Status configureStatus = configureClientSocket(clientFd.get());

                if (!configureStatus.isOk()) {
                    return AcceptClientResult::failed(std::move(configureStatus));
                }

                return AcceptClientResult::accepted(std::move(clientFd));
            }

            return AcceptClientResult::stopped();
        }

        [[nodiscard]] Core::Status acceptAndServeOneClient(int serverFd, Pipeline::LocalPipeline &pipeline, TcpLineServer::StopRequestedCallback stopRequested) {
            AcceptClientResult clientResult = acceptClient(serverFd, stopRequested);
            if (clientResult.type == AcceptClientResultType::Stopped) {
                    return Core::Status::ok();
            }

            if (clientResult.type == AcceptClientResultType::Accepted) {
                return clientResult.status;
            }

            return serveClient(clientResult.clientFd.get(), pipeline, stopRequested);
        }

        void joinClientWorkers(std::vector<ClientWorkerSlot> &workers) {
            for (ClientWorkerSlot &worker : workers) {
                if (worker.thread.joinable()) {
                    worker.thread.join();
                }
            }

            workers.clear();
        }

        void joinFinishedClientWorkers(std::vector<ClientWorkerSlot> &workers) {
            auto it = workers.begin();

            while (it != workers.end()) {
                if (it->finished->load(std::memory_order_acquire)) {
                    if (it->thread.joinable()) {
                        it->thread.join();
                    }

                    it = workers.erase(it);
                } else {
                    ++it;
                }
            }
        }

        [[nodiscard]] bool hasWorkerCapacity(const TcpLineServerOptions &options, const std::atomic_size_t &activeWorkers) noexcept {
            return activeWorkers.load(std::memory_order_relaxed) < options.maxClientWorkers;
        }

        void rejectClientBecauseLimitReached(int clientFd, TcpLineServer::StopRequestedCallback stopRequested) {
            static_cast<void>(sendAll(clientFd, clientLimitResponse, stopRequested));
        }
    }

    TcpLineServer::TcpLineServer(
        TcpEndpoint endpoint,
        Pipeline::LocalPipeline &pipeline,
        TcpLineServerOptions options
        ) : endpoint_(std::move(endpoint)), pipeline_(pipeline), options_(options) {
    }

    const TcpEndpoint &TcpLineServer::endpoint() const noexcept {
        return endpoint_;
    }

    const TcpLineServerOptions &TcpLineServer::options() const noexcept {
        return options_;
    }

    Core::Status TcpLineServer::runOnce() {
        Core::Status optionsStatus = validateOptions(options_);
        if (!optionsStatus.isOk()) {
            return optionsStatus;
        }

        Core::Result<UniqueFd> serverFd = createListeningSocket(endpoint_);

        if (serverFd.isError()) {
            return serverFd.status();
        }

        UniqueFd listeningSocket = std::move(serverFd).value();

        return acceptAndServeOneClient(listeningSocket.get(), pipeline_, nullptr);
    }

    Core::Status TcpLineServer::run(StopRequestedCallback stopRequested) {
        Core::Status optionsStatus = validateOptions(options_);
        if (!optionsStatus.isOk()) {
            return optionsStatus;
        }

        Core::Result<UniqueFd> serverFd = createListeningSocket(endpoint_);

        if (serverFd.isError()) {
            return serverFd.status();
        }

        UniqueFd listeningSocket = std::move(serverFd).value();
        std::vector<ClientWorkerSlot> workers;
        std::atomic_size_t activeClientWorkers {0};

        while (!isStopRequested(stopRequested)) {
            joinFinishedClientWorkers(workers);

            AcceptClientResult clientResult = acceptClient(listeningSocket.get(), stopRequested);
            if (clientResult.type == AcceptClientResultType::Stopped) {
                break;
            }

            if (clientResult.type == AcceptClientResultType::Failed) {
                joinClientWorkers(workers);

                return clientResult.status;
            }

            UniqueFd connectedClient = std::move(clientResult.clientFd);
            if (!hasWorkerCapacity(options_, activeClientWorkers)) {
                rejectClientBecauseLimitReached(connectedClient.get(), stopRequested);
                continue;
            }

            activeClientWorkers.fetch_add(1, std::memory_order_relaxed);

            auto finished = std::make_shared<std::atomic_bool>(false);

            workers.push_back(
                ClientWorkerSlot {
                    .thread = std::thread(
                        [clientFd = std::move(connectedClient),
                            &pipeline = pipeline_,
                            stopRequested,
                            &activeClientWorkers,
                            finished
                        ]() mutable {
                            try {
                                static_cast<void>(serveClient(clientFd.get(), pipeline, stopRequested));
                            } catch (...) {}

                            activeClientWorkers.fetch_sub(1, std::memory_order_relaxed);

                            finished->store(true, std::memory_order_relaxed);
                        }
                    ),
                    .finished = finished
                }
            );
        }

        joinClientWorkers(workers);

        return Core::Status::ok();
    }
}
