# ADR 0016: Server Lifecycle Cleanup
## Status
Accepted.

## Context
`asterd` previously handled CLI parsing, signal handling, storage ownership, pipeline creation,
and TCP server startup directly in `main.cpp`.

This made the application entry point too responsible for runtime lifecycle behavior.

Before adding concurrency or a more advance event loop, AsterKV needs a clearer
server runtime boundary.

## Decision
AsterKV introduces the `server` module.

The module provides:
- `TcpServerOptions`;
- `SignalShutdownController`;
- `TcpServerRuntime`.

Signal handling is moved out of `apps/asterd/main.cpp`.

`TcpServerRuntime` owns the current default single-node in-memory runtime:
```text
InMemoryStorage
    -> LocalPipeline
    -> TcpLineServer
```

## Rationale
This keeps `asterd` focused on process-level user interaction while server lifecycle
behavior moves into a dedicated module.

The network module remains responsible for socket operations and accepts a stop callback
instead of knowing about process signals directly.

## Consequences
The runtime boundary is now ready for future work:
- server configuration;
- lifecycle hooks;
- graceful shutdown improvements;
- threaded client serving;
- event-loop based networking;
- persistent storage runtime.
