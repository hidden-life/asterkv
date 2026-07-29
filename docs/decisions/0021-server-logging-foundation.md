# ADR 0021: Server Logging Foundation with spdlog
## Status
Accepted.

## Context
AsterKV has TCP server runtime, configuration file support, client worker threads,
client limits, and idle timeouts.

The server needs observable runtime behavior before more server features are added.

The project allows infrastructure dependencies when they do not replace core database
subsystems.

## Decision
AsterKV uses `spdlog` as the initial logging backend.

A new `AsterKV::Logging` module provides a project-level logging facade.

The facade owns a process-wide default logger.

Server and network runtime code use `AsterKV::Logging` functions instead of calling
`spdlog` directly.

## Rationale
`spdlog` is a mature infrastructure logging backend.

A project-level facade keeps direct dependency usage concentrated in one module.

A default logger avoids passing logger objects through all server option structures.

This keeps runtime option objects focused on actual server configuration.

## Dependency boundary
AsterKV still does not use external libraries for the core systems intentionally implemented
in-house:
- networking;
- WAL;
- storage engine;
- replication;
- Raft;
- custom protocol.

AsterKV still does not use:
- Boost;
- RocksDB.

## Consequences
Server runtime code can log lifecycle events.

The logging backend can be replaced or extended later behind the facade.

Future work may add:
- config-driven log level;
- CLI log level;
- file sinks;
- rotating logs;
- async logging;
- structured fields.
