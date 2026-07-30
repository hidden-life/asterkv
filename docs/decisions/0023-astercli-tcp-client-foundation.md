# ADR 0023: astercli TCP Client Foundation
## Status
Accepted.

## Context
AsterKV has a TCP server and an in-memory command pipeline.

Before the first release, the project needs a real client-server workflow that does
not depend on `nc`.

`astercli` already supports local command execution, but it needs TCP command execution against
`asterd`.

## Decision
AsterKV adds `Network::TcpLineClient`.

`astercli` adds:
```bash
astercli --connect <host:port> <command>
```

The TCP client sends one command per connection and prints the raw protocol response.

## Rationale
A one-command-per-connection client is simple, deterministic, and easy to test.

It fits the current line-based protocol foundation.

It also avoids designing connection pooling or REPL semantics too early.

## Consequences
AsterKV now has a real client-server smoke workflow:
```bash
asterd --listen 127.0.0.1:7721
astercli --connect 127.0.0.1:7721 PING
```

Future work may add:
- `astercli` TCP REPL mode;
- response decoding and pretty-printing;
- client timeout CLI options;
- persistent connections;
- authentication.
