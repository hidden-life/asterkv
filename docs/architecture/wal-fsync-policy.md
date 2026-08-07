# WAL fsync Policy
AsterKV WAL writer supports configurable sync policy.

## Scope
The sync policy controls when the WAL writer calls `fsync`.

## Supported policies
```text
none
fsync_on_flush
fsync_every_write
```

### `none`
The writer appends records without calling `fsync`.

This is fastest, but provides the weakest crash-safety behavior.

### `fsync_on_flush`
The writer calls `fsync` when `flush()` or `close()` is called.

### `fsync_every_write`
The writer calls `fsync` after every appended WAL record.

This is the default policy.

## Linux implementation
The WAL writer uses a Linux file descriptor and writes through `write()`.

The writer uses `fsync()` for sync operations.

## Important durability note
`fsync_every_write` is stronger than buffered writes, but this step still does
not make AsterKV production durable.

Missing pieces include:
- checksum;
- WAL corruption diagnostics;
- directory sync policy;
- snapshots;
- compaction;
- recovery policy.
