# Known Limitations in AsterKV v0.3.0

AsterKV v0.3.0 introduces the WAL foundation and optional `asterd` WAL recovery.

It is not production-ready.

## WAL durability

The WAL writer can append records to a file.

Current limitations:

- no fsync policy;
- no crash-consistency guarantee;
- no checksum validation;
- no corruption recovery policy;
- no partial recovery policy;
- no segment rotation;
- no WAL compaction.

## Recovery

`asterd` can replay a configured WAL file on startup.

Current limitations:

- recovery is fail-fast;
- corrupted WAL lines fail startup;
- there is no repair mode;
- there is no recovery report;
- there is no skip-corrupted-record mode;
- there is no snapshot fallback.

## Storage

Storage is still in-memory at runtime.

The WAL can rebuild state on startup when configured, but the active storage engine remains memory-backed.

Current limitations:

- no disk-backed storage engine;
- no LSM tree;
- no B-tree;
- no page cache;
- no compaction;
- no snapshots.

## Server integration

WAL is optional.

Without `--wal-file` or `wal_file`, `asterd` uses plain in-memory storage.

Current limitations:

- no automatic WAL path;
- no data directory management;
- no WAL rotation;
- no WAL size limits;
- no admin command for WAL status.

## Distributed systems

Not included yet:

- replication;
- Raft;
- clustering;
- membership;
- leader election;
- consistency levels.

## Security

Not included yet:

- authentication;
- authorization;
- TLS;
- user management.

## Performance

No performance claims are made for v0.3.0.

The current WAL writer opens the file per append, which is simple and testable
but not the final high-performance design.
