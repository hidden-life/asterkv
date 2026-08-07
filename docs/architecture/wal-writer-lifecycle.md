# WAL Writer Lifecycle
AsterKV WAL writer owns a long-lived append file handle.

## Scope
This step changes the WAL writer from per-append file opening to an explicit writer lifecycle.

The writer can:
- open the WAL file;
- append multiple records;
- flush the stream;
- close the file;
- report whether the file handle is open.

## API
```text
WalFileWriter::open(...)
WalFileWriter::appendRecord(...)
WalFileWriter::flush()
WalFileWriter::close()
WalFileWriter::isOpen()
```

## Compatibility helper
The helper remains available:
```text
appendWalRecordToFile(...)
```

It creates a temporary writer, appends one record, and closes the writer.

## WAL-backed storage
`WalBackedStorage` uses a long-lived `WalFileWriter` instance.

This avoids opening the WAL file for every mutation.

## Durability note
This step does not add `fsync`.

`flush()` flushes the C++ stream only.

Operating-system-level sync policy is handled by a later step.

## Sync policy
The writer lifecycle is integrated with WAL sync policy.

The writer can call `fsync`:
- never;
- on explicit flush/close;
- after every appended WAL record.

The sync policy is documented in:
```text
docs/architecture/wal-fsync-policy.md
```
