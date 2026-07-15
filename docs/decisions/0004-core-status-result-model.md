# ADR 0004: Core Status and Result Model
## Status
Accepted.

## Context
AsterKV will contain multiple low-level modules:
- networking;
- protocol parsing;
- command dispatching;
- storage;
- WAL;
- snapshotting;
- replication;
- Raft.

These modules need a consistent way to report recoverable errors.

Using raw strings, booleans, integer return codes, or exceptions inconsistently
would make module boundaries harder to maintain.

## Decision
AsterKV introduces:
- `AsterKV::Core::Status`
- `AsterKV::Core::Result<T>`

`Status` is used for operations that do not return a value.

`Result<T>` is used for operations that either return a value or fail with a
`Status`.

`Result<void>` is not supported. Use `Status`.

`Result<Status>` is not supported. Use `Status`.

## Rationale
The explicit status/result model makes error handling visible in function signatures.

It also avoids exception-based control flow in expected hot-path errors.

## Consequences
Future modules should use `Status` and `Result<T>` for recoverable errors.

Module-specific error codes can be introduced later, but only when needed by implemented
functionality.
