# ADR 0019: TCP Idle Client Timeout
## Status
Accepted.

## Context
AsterKV has a thread-per-client TCP server and a maximum active client worker limit.

A client can still connect and stay idle, keeping a worker slot occupied.

The server needs a basic idle connection policy before more advanced connection
lifecycle management is introduced.

## Decision
AsterKV adds a TCP client idle timeout.

The timeout is represented in:
- `Network::TcpLineServerOptions`;
- `Server::TcpServerOptions`.

`asterd` exposes it through:
```bash
asterd --listen <host:port> --idle-timeout <seconds>
```

The default value is 300 seconds.

When a client is idle for the configured duration, the client worker exits and the socket
is closed.

## Rationale
This protects the thread-per-client server from indefinitely idle clients.

The timeout also ensures that active worker slots can ve released without requiring
client-side disconnects.

## Consequences
Idle clients are disconnected silently.

This is acceptable for the current line-based protocol foundation.

Future work may introduce explicit timeout responses, metrics, and configurable
per-client policies.
