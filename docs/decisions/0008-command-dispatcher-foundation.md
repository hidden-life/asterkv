# ADR 0008: Command Dispatcher Foundation
## Status
Accepted.

## Context
AsterKV already has:
- a command model;
- a protocol parser;
- an in-memory storage implementation.

The project needs a layer that connects parsed commands to storage operations
without coupling protocol parsing directly to storage.

## Decision
AsterKV introduces the `execution` module.

The module provides:
- `CommandResponse`
- `CommandDispatcher`

The dispatcher accepts `CommandRequest`and executes it against a `StorageEngine`.

## Rationale
This keeps the command lifecycle layered:
```text
protocol text
    -> protocol parser
    -> command request
    -> command dispatcher
    -> storage engine
    -> command response
```

The dispatcher becomes the boundary where command semantics are implemented.

## Consequences
Future networking code will not call storage directly.

Future protocol response serialization will consume `CommandResponse`.

The initial dispatcher is synchronous and single-node only.
