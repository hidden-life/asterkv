# Configurable Logging Options
AsterKV supports configurable server log level.

## Supported levels
```text
debug
info
war
warning
error
critical
off
```

`warning` is accepted as an alias for `warn`.

## Default
The default log level is:
```text
info
```

## CLI
For TCP listen mode:
```bash
./build/debug/apps/asterd/asterd --listen 127.0.0.1:7721 --log-level debug
```

## Config file
```ini
listen = 127.0.0.1:7721
max_clients = 128
idle_timeout_seconds = 300
log_level = info
```

Run:
```bash
./build/debug/apps/asterd/asterd --config config/asterd.conf
```

## Config and CLI modes
At this stage, `--config` and `--listen` are separate entry points.

CLI overrides on top of `--config` are not supported yet.

Examples that are not supported yet:
```bash
asterd --config config/asterd.conf --log-level debug
```

## Runtime behavior
`asterd` initializes the process-wide logging facade using the configured log level
before starting `TcpServerRuntime`.

The runtime and networking code use `AsterKV::Logging` facade calls and do not call
`spdlog` directly.
