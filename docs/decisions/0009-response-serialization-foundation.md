# ADR 0009: Response Serialization Foundation
## Status
Accepted.

## Context
AsterKV already has:
- command parsing;
- command dispatching;
- in-memory storage;
- command response objects.

The project needs a protocol-level serialization layer that converts command
execution results into wire-compatible textual responses.

## Decision
AsterKV introduces response serialization in the `protocol` module.

The initial serializer supports:
- simple string responses;
- bulk string responses;
- integer responses;
- status/error responses.

The initial format uses CRLF endings.

## Rationale
Response serialization belongs to the protocol boundary.

Keeping it separate from command execution ensures that execution code remains
independent of wire-format decisions.

## Consequences
Future server and CLI code can execute commands and serialize their results without
depending on storage details.

The initial format is textual and not yet binary-frame-based.
