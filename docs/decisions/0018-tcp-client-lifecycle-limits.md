# ADR 0018: TCP Client Lifecycle Limits
## Status
Accepted.

## Context
AsterKV introduces a thread-per-client TCP serving model.

That model allows concurrent clients, but it can create an unbounded number of worker
threads if many clients connect.

The project needs a simple lifecycle limit before introducing a worker pool or event-loop
based networking.

## Decision
AsterKV adds a maximum active client worker limit.

The default limit is 128 active workers.

The limit is represented in:
- `Network::TcpLineServerOptions`;
- `Server::TcpServerOptions`.

`asterd` exposes the limit through:
```bash
asterd --listen <host:port> --max-clients <count>
```

When the limit is reached, the server sends an unavailable protocol error and closes
the client connection.

## Rationale
This keeps the thread-per-client model bounded.

The rejection response makes the behavior observable to clients and integration tests.

## Consequences
The TCP server now has a basic admission control mechanism.

This is still not a full production lifecycle policy.

Future work may introduce:
- worker pools;
- event-loop based serving;
- queue-based admission control;
- server metrics;
- configurable idle timeouts.
