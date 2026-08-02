# ADR 0034: WAL Replay and WAL-backed Storage Foundation

## Status

Accepted.

## Context

AsterKV now has:

- WAL record model;
- WAL serialization;
- WAL file writer;
- WAL file reader.

The next step is applying WAL records back into storage and introducing a small WAL-backed storage layer.

## Decision

AsterKV adds:

- `WalReplayTarget`;
- record/list/file replay helpers;
- an in-memory replay target;
- `WalBackedStorage`.

`WalBackedStorage` writes the WAL record before applying the mutation to in-memory storage.

## Rationale

Replay should be decoupled from file reading and concrete storage details.

A small WAL-backed storage layer validates the write/recover flow without connecting it to `asterd` yet.

This keeps server integration as a separate step.

## Consequences

The core WAL/storage foundation can now:

```text
write mutation to WAL
apply mutation to storage
read WAL file
replay records
recover in-memory state
```

This still does not mean AsterKV has production-grade durable storage.

Future work should add:
- `asterd` optional WAL path;
- startup recovery;
- fsync policy;
- corruption handling;
- checksums;
- snapshots.
