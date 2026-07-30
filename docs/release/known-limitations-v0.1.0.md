# Known limitations in AsterKV v0.1.0
AsterKV v0.1.0 is a foundation release.

It is not production-ready.

## Storage
The storage engine is in-memory only.

Data is lost when the server exits.

No included yet:
- persistent storage;
- WAL;
- snapshots;
- compaction;
- recovery.

## Networking
The TCP server is blocking and foundational.

Not included yet:
- async event loop;
- worker pool;
- backpressure model;
- TLS;
- authentication;
- protocol negotiation.

## Client
`astercli` supports:
- local command mode;
- single-command TCP mode;
- TCP REPL mode.

Current limitations:
- REPL opens one TCP connection per command;
- no command history;
- no autocompletion;
- no pretty response rendering;
- no persistent session state;
- no client config file.

## Protocol
The protocol is line-based and foundational.

Not included yet:
- binary protocol;
- pipelining;
- streaming response;
- negotiated protocol versions;
- advanced error model.

## Commands
Only the initial command set is supported:
```text
PING
SET
GET
DEL
EXISTS
```

## Distributed systems
Not included yet:
- replication;
- Raft;
- clustering;
- membership;
- leader election;
- consistency levels.

## Observability
The release includes server logging, but not:
- metrics;
- tracing;
- structured logs;
- file log sinks;
- rotating logs;
- admin diagnostics API.

## Performance
No performance claims are made for v0.1.0.

Benchmarking is not part of this release.
