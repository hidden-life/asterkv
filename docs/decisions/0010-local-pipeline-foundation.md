# ADR 0010: Local Pipeline Foundation
## Status
Accepted.

## Context
AsterKV now has separate modules for:
- command modeling;
- protocol parsing;
- storage;
- command execution;
- response serialization.

Before adding TCP networking, the project needs an in-process pipeline that proves
layers work together.

## Decision
AsterKV introduces the `pipeline` module.

The initial pipeline provides `LocalPipeline`.

`LocalPipeline::processLine()` accepts one textual command line and returns one serialized
protocol response.

## Rationale
The local pipeline allows end-to-end command execution without a server socket.

This keeps networking out of the first integration step and makes behavior easy
to test.

## Consequences
Future CLI and TCP server code can reuse the local pipeline.

The current pipeline is synchronous and single-command oriented.
