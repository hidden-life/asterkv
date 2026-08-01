# WAL File Writer Foundation

AsterKV provides a foundational append-only WAL file writer.

## Scope

The writer appends serialized WAL records to a file.

It does not yet provide full durability guarantees.

## Module

The writer lives in:

```text
AsterKV::Wal
```

Public header:
```text
include/asterkv/wal/wal_file_writer.h
```

## API
The writer API provides:
```text
WalFileWriterOptions
WalFileWriter
appendWalRecordToFile(...)
```

## Append-only behavior
Each call appends one serialized WAL record to the target file.

Example content:
```text
AKVWAL1 1 set 757365726e616d65 6a61636b736f6e
AKVWAL1 2 del 757365726e616d65 -
```

## Flush behavior
By default, the writer flushes the stream after each write.

This is controlled by:
```text
flushAfterWrite
```

## Important durability note
`flushAfterWrite` flushes the C++ stream.

It is not the same as `fsync`.

This step does not claim crash-safe durability.

## Current limitations
The writer does not yet provide:
- fsync policy;
- open writer handle reuse;
- segment rotation;
- checksums;
- record batching;
- WAL reader;
- WAL replay;
- server integration;
- recovery flow.
