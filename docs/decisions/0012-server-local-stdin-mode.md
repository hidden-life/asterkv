# ADR 0012: Server Local Stdin Mode
## Status
Accepted.

## Context
AsterKV already has a local pipeline and CLI local execution mode.

Before introducing TCP networking, the server daemon should also expose a simple-server side
execution path.

This allows the server binary to exercise the same command pipeline without sockets.

## Decision
AsterKV adds `asterd --local`.

The mode supports:
- one-command execution through `asterd --local <command>`;
- stdin-driven execution through `asterd --local`.

Both paths use `InMemoryStorage` and `LocalPipeline`.

## Rationale
This provides a server-side integration checkpoint before networking.

The stdin mode allows multi-command sessions in one process, which verifies
stateful behavior without TCP.

## Consequences
The server binary can now execute commands locally.

This mode is not persistent and does not share state before separate processes.

Future TCP server code should reuse the same pipeline boundaries instead of bypassing them.

