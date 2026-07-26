# ADR 0015: TCP Integration Smoke Test Foundation
## Status
Accepted.

## Context
AsterKV now has a blocking sequential TCP server that can serve multiple clients over time.

Unit-style tests cover endpoint parsing, but they do not verify the full TCP path.

The project needs an integration smoke test that exercises the real server binary
and a real TCP client.

## Decision
AsterKV adds a shell-based TCP integration smoke test.

The test:
- starts `asterd --listen` in the background;
- waits until the TCP server accepts connections;
- sends commands through `nc -q 1`;
- verifies protocol responses;
- verifies state is shared across multiple client connections;
- stops the server with `SIGTERM`;
- verifies graceful shutdown.

The test is only registered when both `nc` and `timeout` are available.

## Rationale
A shell smoke test validates behavior that cannot be fully covered by unit tests:
- process startup;
- socket handling;
- client connection;
- command exchange over TCP;
- cross-client in-memory state;
- signal-driven shutdown.

Using `nc -q 1` matches the currently verified local environment.

## Consequences
The test introduces a small amount of environment sensitivity.

The test uses a fixed local port for now.

Future work may introduce dynamic port allocation and stronger test isolation.
