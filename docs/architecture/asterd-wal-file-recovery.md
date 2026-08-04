# `asterd` WAL File Recovery
AsterKV supports an optional WAL file path for `asterd`.

## Scope
When configured, `asterd`:
- reads the WAL file on startup;
- replays WAL records into in-memory storage;
- appends future `SET` and `DEL` mutations to the WAL file.

## CLI
```bash
asterd --listen 127.0.0.1:7721 --wal-file ./data/asterkv.wal
```

## Config
```ini
wal_file = ./data/asterkv.wal
```

## Runtime behavior
Without `wal_file`, `asterd` uses plain in-memory storage.

With `wal_file`, `asterd` uses WAL-backed storage over in-memory storage.

## Recovery behavior
On startup:
```text
read WAL file
deserialize records
replay records in file order
restore in-memory state
```

If the WAL file does not exist yet, startup succeeds and the file is created on the first
mutation.

## Important durability note
This is still a foundation feature.

It does not yet provide production durability guarantees.

Missing pieces include:
- fsync policy;
- checksum validation;
- corruption recovery policy;
- snapshots;
- compaction.
