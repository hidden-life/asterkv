# Known Limitations in AsterKV v0.2.0

AsterKV v0.2.0 is a client UX foundation release.

It is not production-ready.

## CLI

`astercli` now provides pretty output by default, but it does not yet provide:

- public raw output mode;
- command history;
- autocompletion;
- persistent TCP client sessions;
- multiline commands;
- client-side command validation;
- configurable output formats;
- colorized output.

## REPL

The TCP REPL is intentionally simple.

Current limitations:

- one TCP connection per command;
- no persistent REPL protocol session;
- no line editing library;
- no history;
- no autocompletion;
- no command-specific detailed help.

## Protocol rendering

The client renderer supports the protocol response kinds currently produced by AsterKV:

- simple string;
- error;
- integer;
- bulk string.

It does not yet support:

- arrays;
- maps;
- streaming responses;
- typed command-specific renderers;
- table formatting.

## Storage

Storage remains in-memory only.

Data is lost when the server exits.

Not included yet:

- persistent storage;
- WAL;
- snapshots;
- compaction;
- recovery.

## Distributed systems

Not included yet:

- replication;
- Raft;
- clustering;
- membership;
- leader election;
- consistency levels.

## Security

Not included yet:

- authentication;
- authorization;
- TLS;
- user management.

## Observability

Server logging exists, but the project does not yet provide:

- metrics;
- tracing;
- admin diagnostics API;
- file log sinks;
- rotating logs.

## Performance

No performance claims are made for v0.2.0.

Benchmarking is not part of this release.
