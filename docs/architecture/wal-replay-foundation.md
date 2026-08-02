# WAL Replay Foundation
AsterKV provides a foundational WAL replay layer.

## Scope
The replay layer applies `WalRecord` values to a replay target.

It can replay:
- one WAL record;
- a list of WAL records;
- a WAL file read through the WAL file reader.

## Replay target
Replay is decoupled from a concrete storage implementation through:
```text
WalReplayTarget
```

The target receives logical mutations:
```text
set(key, value)
del(key)
```

## Record order
Records are replayed in the same order in which they appear in memory or in the
WAL file.

## Current limitations
The replay layer does not yet provide:
- checksum verification;
- partial recovery policy;
- idempotency validation;
- transaction boundaries;
- compaction;
- snapshots.
