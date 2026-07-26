# ADR 0014: TCP Accept Loop and Graceful Shutdown
## Status
Accepted.

## Context
AsterKV already has a TCP server foundation that can accept and serve one client.

The next step is to keep the server process alive across multiple client connections
while avoiding concurrency complexity.

The server also needs a basic graceful shutdown path for manual operation.

## Decision
AsterKV extends `TcpLineServer` with an accept loop.

The server remains blocking and sequential:
```text
accept
    -> serve one client
    -> accept next client
```

`asterd --listen <host:port>` now runs until a shutdown signal is received.

The daemon installs handlers for:
- `SIGINT`;
- `SIGTERM`.

The signal handler only sets a stop flag. The server checks this flag from blocking
loops interrupted by signals.

## Rationale
A sequential accept loop is the smallest step from one-client TCP support toward
a long-running server.

It allows repeated client sessions while keeping the networking implementation easy to reason
about.

Graceful shutdown is required before adding more complex server lifecycle behavior.

## Consequences
The server can now remain running across multiple client connections.

Only one client is served at a time.

Future work can add:
- multi-client concurrency;
- worker threads;
- event-loop based networking;
- server lifecycle management;
- admin shutdown commands.
