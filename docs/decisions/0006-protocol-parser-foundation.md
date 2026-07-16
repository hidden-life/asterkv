# ADR 0006: Protocol Parser Foundation
## Status
Accepted.

## Context
AsterKV needs a protocol layer that is independent from networking and storage.

The first protocol implementation must be simple enough to test without a TCP server,
but structured enough to evolve into a custom wire protocol later.

## Decision
AsterKV introduces the `protocol` module.

The initial protocol parser is line-based and whitespace-separated.

It exposes:
- `tokenizeCommandLine()`
- `parseCommandLine()`

The parser returns `AsterKV::Command::CommandRequest` using the core `Result<T>` model.

## Rationale
A line-based parser is easy to test and debug before the network layer exists.

Separating protocol parsing from command modeling keeps responsibilities clear:
```text
text line
    -> protocol tokenizer/parser
    -> command request
    -> future dispatcher
    -> future storage engine
```

## Consequences
The initial parser is not binary-safe.

Values containing spaces are not supported yet.

Future protocol work may introduce a frame-based format while keeping `CommandRequest`
as the internal command representation.
