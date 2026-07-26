# Server Runtime Boundary
AsterKV introduces a server runtime boundary between application argument parsing
and low-level network serving.

## Current components
The server module currently provides:
- `TcpServerOptions`;
- `SignalShutdownController`;
- `TcpServerRuntime`.

## Responsibility split
`apps/asterd` is responsible for:
- parsing CLI arguments;
- printing user-facing startup and shutdown messages;
- returning process exit codes.

`AsterKV::Server` is responsible for:
- server runtime options;
- signal-based shutdown handling;
- owning the default in-memory server runtime;
- connecting storage, pipeline, and TCP serving.

`AsterKV::Network` is responsible for:
- TCP endpoint parsing;
- socket binding;
- accept loop;
- client read/write handling.

## Runtime flow
```text
asterd --listen <host:port>
    -> parse TcpEndpoint
    -> create TcpServerOptions
    -> create TcpServerRuntime
    -> install shutdown signal handlers
    -> run TcpLineServer
```

## Current limitations
The current runtime still uses:
- in-memory storage only;
- blocking sequential TCP serving;
- process-global signal handling.

Future work will introduce stronger lifecycle management before adding concurrency,
persistence, and production configuration.
