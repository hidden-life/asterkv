# ADR 0022: Configurable Logging Options
## Status
Accepted.

## Context
AsterKB uses `spdlog` behind the `AsterKV::Logging` facade.

The initial logging foundation used a fixed `info` level.

Server deployments and debugging sessions need a way to control runtime verbosity.

## Decision
AsterKV adds a configurable log level.

The log level is represented in `Server::TcpServerOptions`.

The default level is `info`.

The config file supports:
```ini
log_level = info
```

`asterd --listen` supports:
```bash
asterd --listen <host:port> --log-level <level>
```

Supported values are:
- `debug`;
- `info`;
- `warn`;
- `warning`;
- `error`;
- `critical`;
- `off`.

## Rationale
The log level is server runtime configuration, so it belongs to `TcpServerOptions`.

Keeping it in options allows both CLI and config-file entry points to user the same
runtime path.

## Consequences
`asterd` can now start with different logging verbosity levels.

The project still keeps direct `spdlog` usage behind the `AsterKV::Logging` facade.

Future work may add:
- CLI overriders for config-loaded values;
- file sinks;
- rotating logs;
- per-component loggers;
- structured fields.
