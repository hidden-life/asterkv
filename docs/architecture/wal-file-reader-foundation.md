# WAL File Reader Foundation

AsterKV provides a foundational WAL file reader.

## Scope

The reader reads serialized WAL records from a file and deserializes them into `WalRecord` values.

This step does not replay records into storage.

## Module

The reader lives in:

```text
AsterKV::Wal
```

Public header:
```text
include/asterkv/wal/wal_file_reader.h
```

## API
The reader API provides:
```text
WalFileReader
readWalRecordsFromFile(...)
```

## Behavior
The reader:
- validates that the file path is not empty;
- opens the target file in binary read mode;
- reads the file line by line;
- deserializes each line through the WAL record codec;
- returns records in file order.

## Empty files
An empty WAL file is valid.

It returns an empty record list.

## Invalid lines
Invalid record lines make the read operation fail.

Examples:
- unsupported WAL format version;
- invalid field count;
- invalid sequence number;
- unknown record type;
- invalid hex field;
- empty WAL record line.

## Current limitations
The reader does not yet provide:
- replay into storage;
- partial recovery policy;
- checksum verification;
- segment reading;
- corruption repair;
- recovery diagnostics;
- server integration.
