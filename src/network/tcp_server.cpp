#include <unistd.h>
#include <string>
#include <cerrno>
#include <utility>
#include <cstring>
#include <array>
#include <arpa/inet.h>

#include <asterkv/network/tcp_server.h>
#include <asterkv/core/result.h>
#include <asterkv/pipeline/local_pipeline.h>

#include "asterkv/network/tcp_endpoint.h"

namespace AsterKV::Network {
    namespace {
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

        [[nodiscard]] Core::Status systemUnavailableError(std::string_view context) {
            std::string message {context};
            message.append(": ");
            message.append(std::strerror(errno));

            return Core::Status::unavailable(std::move(message));
        }

        [[nodiscard]] Core::Status sendAll(int clientFd, std::string_view bytes) {
            std::size_t totalSent = 0;

            while (totalSent < bytes.size()) {
                const ssize_t sentCount = ::send(
                    clientFd,
                    bytes.data() + totalSent,
                    bytes.size() - totalSent,
                    MSG_NOSIGNAL
                );

                if (sentCount < 0) {
                    if (errno == EINTR) {
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

        [[nodiscard]] Core::Status processLineAndSendResponse(int clientFd, Pipeline::LocalPipeline &pipeline, std::string_view rawLine) {
            std::string_view line = rawLine;

            if (!line.empty() && line.back() == '\r') {
                line.remove_suffix(1);
            }

            const std::string response = pipeline.processLine(line);

            return sendAll(clientFd, response);
        }

        [[nodiscard]] Core::Status processBufferedLines(int clientFd, Pipeline::LocalPipeline &pipeline, std::string &input) {
            std::size_t newlinePosition = input.find('\n');
            while (newlinePosition != std::string::npos) {
                const std::string line = input.substr(0, newlinePosition);
                Core::Status status = processLineAndSendResponse(clientFd, pipeline, line);
                if (!status.isOk()) {
                    return status;
                }

                input.erase(0, newlinePosition + 1);
                newlinePosition = input.find('\n');
            }

            return Core::Status::ok();
        }

        [[nodiscard]] Core::Status serveClient(int clientFd, Pipeline::LocalPipeline &pipeline) {
            std::array<char, 4096> buffer {};
            std::string pendingInput;

            while (true) {
                const ssize_t readCount = ::recv(clientFd, buffer.data(), buffer.size(), 0);
                if (readCount < 0) {
                    if (errno == EINTR) {
                        continue;
                    }

                    return systemUnavailableError("failed to read client connection");
                }

                if (readCount == 0) {
                    break;
                }

                pendingInput.append(buffer.data(), static_cast<std::size_t>(readCount));
                Core::Status status = processBufferedLines(clientFd, pipeline, pendingInput);

                if (!status.isOk()) {
                    return status;
                }
            }

            if (!pendingInput.empty()) {
                return processLineAndSendResponse(clientFd, pipeline, pendingInput);
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
    }

    TcpLineServer::TcpLineServer(TcpEndpoint endpoint, Pipeline::LocalPipeline &pipeline) : endpoint_(std::move(endpoint)), pipeline_(pipeline) {
    }

    const TcpEndpoint & TcpLineServer::endpoint() const noexcept {
        return endpoint_;
    }

    Core::Status TcpLineServer::runOnce() {
        Core::Result<sockaddr_in> socketAddr = makeSocketAddress(endpoint_);
        if (socketAddr.isError()) {
            return socketAddr.status();
        }

        const int rawServerFd = ::socket(AF_INET, SOCK_STREAM, 0);
        if (rawServerFd < 0) {
            return systemUnavailableError("failed to create server socket");
        }

        UniqueFd serverFd {rawServerFd};
        int reuseAddress = 1;

        if (::setsockopt(serverFd.get(), SOL_SOCKET, SO_REUSEADDR, &reuseAddress, sizeof(reuseAddress)) < 0) {
            return systemUnavailableError("failed to configure server socket");
        }

        sockaddr_in addr = socketAddr.value();

        if (::bind(serverFd.get(), reinterpret_cast<const sockaddr*>(&addr), sizeof(addr)) < 0) {
            return systemUnavailableError("failed to bind server socket");
        }

        constexpr int backlog = 16;
        if (::listen(serverFd.get(), backlog) < 0) {
            return systemUnavailableError("failed to listen on server socket");
        }

        sockaddr_in clientAddr {};
        socklen_t clientAddrLen = sizeof(clientAddr);

        const int rawClientFd = ::accept(
            serverFd.get(),
            reinterpret_cast<sockaddr*>(&clientAddr),
            &clientAddrLen
        );

        if (rawClientFd < 0) {
            return systemUnavailableError("failed to accept client connection");
        }

        UniqueFd clientFd {rawClientFd};

        return serveClient(clientFd.get(), pipeline_);
    }
}
