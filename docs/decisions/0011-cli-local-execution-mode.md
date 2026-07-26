# ADR 0011: CLI Local Execution Mode
## Status
Accepted.

## Context
AsterKV now has a local in-process pipeline that connects protocol parsing, command
dispatching, storage and response serialization.

Before adding a TCP server, the project needs a simple operational entry point for
manually exercising the pipeline.

## Decision
AsterKV adds `astercli local`.

The local mode supports:
- single-command execution;
- interactive REPL execution.

Both modes use `InMemoryStorage` and `LocalPipeline`.

## Rationale
This provides an early end-to-end user-facing workflow without introducing networking
complexity.

The REPL mode is useful because state is preserved during one process lifetime.

## Consequences
Separate `astercli local <command>` invocations do not share state.

The CLI remains dependency-free for now. CLI11 will be introduced later when argument
parsing becomes complex enough to justify it.
