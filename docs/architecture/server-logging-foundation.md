# Server Logging Foundation
AsterKV uses `spdlog` as the initial server logging backend.

## Dependency boundary
`spdlog` is an infrastructure dependency.

It does not replace AsterKV core systems that are intentionally implemented in-house:
- networking;
- storage engine;
- WAL;
- replication;
- Raft;
- custom protocol.

AsterKV still does not use:
- Boost;
- RocksDB.

## Logging facade
The project exposes logging through `AsterKV::Logging`.

Runtime code should call:
```c++
AsterKV::Logging::info("message");
AsterKV::Logging::warn("message");
AsterKV::Logging::error("message");
```

Runtime code should not call `spdlog` directly.

## Default logger
The logging module owns a process-wide default logger.

`asterd` initializes it on startup:
```c++
AsterKV::Logging::initializeDefaultConsoleLogger("asterd");
```

The default logger is used by server and network runtime code.

## Why singleton is acceptable here
Logging is cross-cutting infrastructure.

A process-wide logger avoids passing logger objects through every runtime options object.

The singleton is kept inside the logging module and can be reset in tests.

This rule does not apply to core database systems such as storage, networking, WAL,
Raft, replication, or protocol handling.

## Current log format
```text
[2026-01-01 12:00:00.000] [info] [asterd] starting TCP server runtime
```

## Current events
The TCP server currently logs:
- listen mode startup;
- signal handler installation;
- TCP runtime startup;
- accept loop startup;
- accepted client connections;
- client limit rejections;
- idle client disconnects;
- accept loop shutdown;
- runtime shutdown.

## Current limitations
The current implementation does not yet support:
- log level from config;
- log level from CLI;
- file sinks;
- rotation logs;
- structured fields;
- async logging;
- per-component logger names.
