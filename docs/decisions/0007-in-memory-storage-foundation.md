# ADR 0007: In-memory Storage Foundation
## Status
Accepted.

## Context
AsterKV needs an execution layer for key-value commands before networking, WAL, replication
and Raft are introduced.

The first storage implementation must be simple, deterministic, and easy to test.

## Decision
AsterKV introduces the `storage` module.

The initial module provides:
- `StorageEngine`, an abstract key-value storage interface;
- `InMemoryStorage`, an in-memory implementation.

The first supported operations are:
- `set`
- `get`
- `remove`
- `exists`

## Rationale
An in-memory storage layer provides a clean foundation for future command dispatching
and server execution.

It keeps storage behavior independent from protocol parsing and networking.

## Consequences
The storage layer can now be used by future command dispatching.

The initial implementation is not persistent and does not provide concurrency
guarantees.

Persistence, WAL, snapshots, TTL, and concurrency will be added later.
