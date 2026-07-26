# TCP Accept Loop Foundation
AsterKV extends the TCP server foundation with a blocking accept loop and
graceful shutdown.

## Current behavior
`asterd --listen <host:port>` starts a blocking TCP server.

The server:
- binds to the configured IPv4 endpoint;
- listens for client connections;
- accepts clients in a loop;
- serves one client at a time;
- processes newline-delimited commands through `LocalPipeline`;
- keeps shared in-memory state for the lifetime of the server process;
- stops when SIGINT or SIGTERM received.

## Sequential client model
The current server is intentionally sequential.

It does now spawn threads and does not use an event loop.

```text
accept client
    -> serve client until disconnect
    -> accept next client
```

This keeps networking behavior simple before concurrency and event-driven I/O
are introduced.

## Graceful shutdown
`asterd` installs signal handlers for:
- `SIGINT`;
- `SIGTERM`.

When a shutdown signal received, the server exits the accept or receive loop and
returns successfully.

## Manual testing
Use `nc -q 1` when testing one-shot commands:
```bash
printf 'PING\n' | nc -q 1 127.0.0.1 7721
```

The `-q 1` flag asks netcat to close the connection shortly after stdin reaches EOF.

Without this behavior, the server may keep serving the first client and will not accept
the next one yet because the current server is sequential.

## Current limitations
The current TCP server does not provide:
- concurrent clients;
- non-blocking sockets;
- event loop;
- socket timeouts;
- TLS;
- authentication;
- persistence;
- WAL;
- replication;
- Raft integration.

Those features will be added incrementally.
