# WAL Record Model

The WAL record model defines the logical mutations that can later be persisted and replayed.

## Module

The model lives in:

```text
AsterKV::Wal
```

## Public header
```text
include/asterkv/wal/wal_record.h
```

## Record type
A WAL record contains:
```text
sequenceNumber
type
key
value
```

## Supported record types
```text
Set
Del
```

## Set record
A `Set` record represents:
```text
SET <key> <value>
```

Validation rules:
- sequence number must be greater than zero;
- key must not be empty;
- value must not be empty.

## Del record
A `Del` record represents:
```text
DEL <key>
```

Validation rules:
- sequence number must be greater than zero;
- key must not be empty;
- value must not be empty.

## Serialization

The logical record model can be serialized through the WAL serialization foundation.

The current serialized format is documented in:

```text
docs/architecture/wal-serialization-format.md
```

Serialization is separate from file writing.

A serialized WAL record is not durable until a future WAL writer persists it to storage.

## Current limitations
The record model does not yet define:
- serialization format;
- checksums;
- timestamps;
- transaction boundaries;
- record batching;
- segment identifiers;
- replay behavior.
