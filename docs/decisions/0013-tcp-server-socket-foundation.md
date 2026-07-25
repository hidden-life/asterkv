# ADR 0013: TCP Server Socket Foundation
## Status
Accepted.

## Context
AsterKV already has a complete local in-process command pipeline and server local
stdin mode.

The next step is to expose the pipeline over a real TCP socket without introducing
concurrency or event-loop complexity yet.

## Decision
AsterKV introduces the `network` module.

The initial module provides:
- `TcpEndpoint`;
- `parseTcpEndpoint()`;
- `TcpLineServer`.

`asterd --listen <host:port>` starts a blocking TCP server that accepts one client,
processes line-based commands, writes responses, and exits after the client disconnects.

## Rationale
This is the smallest useful TCP integration step.

It validates the network boundary while keeping command execution logic inside
`LocalPipeline`.

## Consequences
The server can now be exercised through standard TCP clients such as `nc`.

The first implementation is intentionally blocking and single-client.

Future steps will add a multi-client accept loop, shutdown handling, and concurrency/event-loop.


