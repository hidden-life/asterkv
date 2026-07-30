# ADR 0024: astercli TCP REPL Foundation
## Status
Accepted.

## Context
AsterKV has a TCP server and a single-command TCP mode in `astercli`.

Manual testing becomes easier if users can connect once from a CLI process and type
multiple commands interactively.

## Decision
`astercli --connect <host:port>` without a command starts TCP REPL mode.

`astercli --connect <host:port> <command>` keeps single-command TCP mode.

The REPL uses the existing `Network::TcpLineClient`.

Each REPL command opens a new TCP connection.

The REPL exits on `exit` or `quit`.

## Rationale
Using the existing one-command TCP client keeps this step small and reliable.

It avoids introducing persistent connection semantics before the server protocol
and client response model are finalized.

It also allows the REPL to reuse the same TCP error handling path as single-command mode.

## Consequences
Users can manually test AsterKV with:
```bash
astercli --connect 127.0.0.1:7721
```

The REPL is not yet a persistent protocol session.

Future work may add:
- persistent TCP connection reuse;
- command history;
- autocompletion;
- pretty response rendering;
- client timeout options.
