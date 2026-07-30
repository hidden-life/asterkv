#include <cstring>
#include <asterkv/network/tcp_client.h>

#include <string>
#include <string_view>
#include <unistd.h>
#include <arpa/inet.h>
#include <array>

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

            UniqueFd(UniqueFd &&other) noexcept : fd_(other.release()) {}

            UniqueFd& operator=(UniqueFd &&other) noexcept {
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

        [[nodiscard]] Core::Status validateCommandLine(std::string_view commandLine) {
            if (commandLine.empty()) {
                return Core::Status::invalidArgument("TCP command line must not be empty");
            }

            if (commandLine.find('\n') != std::string_view::npos || commandLine.find('\r') != std::string_view::npos) {
                return Core::Status::invalidArgument(
                    "TCP command line must not contain newline characters"
                );
            }

            return Core::Status::ok();
        }

        [[nodiscard]] Core::Status validateOptions(const TcpLineClientOptions &options) {
            if (options.ioTimeoutSeconds == 0) {
                return Core::Status::invalidArgument(
                    "TCP client I/O timeout seconds must be greater than zero"
                );
            }

            if (options.maxResponseBytes == 0) {
                return Core::Status::invalidArgument(
                    "TCP client max response bytes must be greater than zero"
                );
            }

            return Core::Status::ok();
        }

        [[nodiscard]] Core::Status configureSocketTimeouts(
            int fd,
            std::uint32_t seconds
        ) {
            timeval timeout {};
            timeout.tv_sec = static_cast<decltype(timeval::tv_sec)>(seconds);
            timeout.tv_usec = 0;

            if (::setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
                return systemUnavailableError("failed to configure TCP client receive timeout");
            }

            if (::setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
                return systemUnavailableError("failed to configure TCP client send timeout");
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
                    Core::Status::invalidArgument("connect host must be a valid IPv4 address")
                );
            }

            return Core::Result<sockaddr_in>::success(address);
        }

        [[nodiscard]] Core::Result<UniqueFd> connectSocket(const TcpEndpoint &endpoint, const TcpLineClientOptions &options) {
            Core::Result<sockaddr_in> socketAddr = makeSocketAddress(endpoint);
            if (socketAddr.isError()) {
                return Core::Result<UniqueFd>::failure(socketAddr.status());
            }

            const int rawFd = ::socket(AF_INET, SOCK_STREAM, 0);
            if (rawFd < 0) {
                return Core::Result<UniqueFd>::failure(systemUnavailableError("failed to create TCP client socket"));
            }

            UniqueFd fd {rawFd};

            Core::Status timeoutStatus = configureSocketTimeouts(fd.get(), options.ioTimeoutSeconds);
            if (!timeoutStatus.isOk()) {
                return Core::Result<UniqueFd>::failure(timeoutStatus);
            }

            sockaddr_in addr = socketAddr.value();
            if (::connect(fd.get(), reinterpret_cast<const sockaddr*>(&addr), sizeof(addr)) < 0) {
                return Core::Result<UniqueFd>::failure(systemUnavailableError("failed to connect TCP server"));
            }

            return Core::Result<UniqueFd>::success(std::move(fd));
        }

        [[nodiscard]] bool isTemporarySocketWaitError() noexcept {
            return errno == EAGAIN || errno == EWOULDBLOCK;
        }

        [[nodiscard]] Core::Status sendAll(int fd, std::string_view bytes) {
            std::size_t totalSent = 0;

            while (totalSent < bytes.size()) {
                const ssize_t sentCount = ::send(fd, bytes.data() + totalSent, bytes.size() - totalSent, MSG_NOSIGNAL);
                if (sentCount < 0) {
                    if (errno == EINTR || isTemporarySocketWaitError()) {
                        continue;
                    }

                    return systemUnavailableError("failed to write TCP client request");
                }

                if (sentCount == 0) {
                    return Core::Status::unavailable("failed to write TCP client request: zero bytes written");
                }

                totalSent += static_cast<std::size_t>(sentCount);
            }

            return Core::Status::ok();
        }

        [[nodiscard]] Core::Status sendCommand(int fd, std::string_view commandLine) {
            Core::Status commandStatus = sendAll(fd, commandLine);
            if (!commandStatus.isOk()) {
                return commandStatus;
            }

            commandStatus = sendAll(fd, "\n");
            if (!commandStatus.isOk()) {
                return commandStatus;
            }

            if (::shutdown(fd, SHUT_WR) < 0) {
                return systemUnavailableError("failed to shutdown TCP client write side");
            }

            return Core::Status::ok();
        }

        [[nodiscard]] Core::Result<std::string> readResponse(int fd, std::size_t maxResponseBytes) {
            std::array<char, 4096> buffer {};
            std::string response;

            while (true) {
                const ssize_t readCount = ::recv(fd, buffer.data(), buffer.size(), 0);
                if (readCount < 0) {
                    if (errno == EINTR || isTemporarySocketWaitError()) {
                        continue;
                    }

                    return Core::Result<std::string>::failure(systemUnavailableError("failed to read TCP client response"));
                }

                if (readCount == 0) {
                    break;
                }

                response.append(buffer.data(), static_cast<std::size_t>(readCount));

                if (response.size() > maxResponseBytes) {
                    return Core::Result<std::string>::failure(
                        Core::Status::unavailable("TCP client response exceeds maximum size")
                    );
                }
            }

            if (response.empty()) {
                return Core::Result<std::string>::failure(Core::Status::unavailable("TCP client received empty response"));
            }

            return Core::Result<std::string>::success(std::move(response));
        }
    }

    TcpLineClient::TcpLineClient(TcpEndpoint endpoint, TcpLineClientOptions options) : endpoint_(std::move(endpoint)), options_(options) {
    }

    const TcpEndpoint &TcpLineClient::endpoint() const noexcept {
        return endpoint_;
    }

    const TcpLineClientOptions &TcpLineClient::options() const noexcept {
        return options_;
    }

    Core::Result<std::string> TcpLineClient::sendCommandLine(std::string_view commandLine) const {
        Core::Status optionsStatus = validateOptions(options_);
        if (!optionsStatus.isOk()) {
            return Core::Result<std::string>::failure(optionsStatus);
        }

        Core::Status commandStatus = validateCommandLine(commandLine);
        if (!commandStatus.isOk()) {
            return Core::Result<std::string>::failure(commandStatus);
        }

        Core::Result<UniqueFd> fd = connectSocket(endpoint_, options_);
        if (fd.isError()) {
            return Core::Result<std::string>::failure(fd.status());
        }

        UniqueFd connected = std::move(fd).value();

        Core::Status sendStatus = sendCommand(connected.get(), commandLine);
        if (!sendStatus.isOk()) {
            return Core::Result<std::string>::failure(sendStatus);
        }

        return readResponse(connected.get(), options_.maxResponseBytes);
    }
}
