#ifndef ASTERKV_CONFIG_SERVER_CONFIG_H
#define ASTERKV_CONFIG_SERVER_CONFIG_H

#include <asterkv/core/result.h>
#include <asterkv/server/tcp_server_options.h>

#include <string_view>

namespace AsterKV::Config {
    [[nodiscard]] Core::Result<Server::TcpServerOptions> parseServerConfig(std::string_view config);

    [[nodiscard]] Core::Result<Server::TcpServerOptions> loadServerConfig(std::string_view path);
}

#endif //ASTERKV_CONFIG_SERVER_CONFIG_H
