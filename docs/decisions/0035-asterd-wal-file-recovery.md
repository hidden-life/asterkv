# ADR 0035: asterd Optional WAL File and Startup Recovery

## Status

Accepted.

## Context

AsterKV has a WAL record model, serialization, file writer, file reader, replay, and WAL-backed storage foundation.

The next step is connecting this foundation to `asterd`.

## Decision

`asterd` accepts an optional WAL file path through:

- CLI: `--wal-file <path>`;
- config: `wal_file = <path>`.

When the option is absent, `asterd` continues to use plain in-memory storage.

When the option is present, `asterd` uses `WalBackedStorage`.

`WalBackedStorage` writes WAL records before applying `SET` and `DEL` mutations to in-memory storage.

On startup, `asterd` replays the WAL file before accepting TCP clients.

## Consequences

AsterKV can now preserve basic `SET` and `DEL` state across server restarts when a WAL file is configured.

This is not yet production-grade durability.

Future work should add:

- fsync policy;
- checksums;
- recovery diagnostics;
- snapshotting;
- compaction.
