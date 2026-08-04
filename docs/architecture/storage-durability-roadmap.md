# Storage Durability Roadmap

AsterKV 0.3.0 starts the storage durability and WAL foundation milestone.

## Goal

The goal of this milestone is to introduce the foundation required for future durable single-node storage.

This milestone does not immediately make AsterKV production-durable.

## Planned development path

The planned path is:

1. WAL record model.
2. WAL record serialization.
3. WAL file writer.
4. WAL file reader.
5. WAL replay into in-memory storage.
6. Optional `asterd` WAL path.
7. Release preparation for `v0.3.0`.

Implemented foundations:
- WAL record model;
- WAL record validation;
- WAL record serialization;
- WAL record deserialization;
- WAL file writer foundation;
- WAL file reader foundation.
- WAL replay foundation.
- WAL-backed in-memory storage foundation.
- Optional `asterd` WAL file path.
- Startup recovery from WAL file.
- TCP mutation persistence through WAL-backed storage.

Not implemented yet:
- replay;
- `asterd` integration;
- fsync policy;
- durability guarantees.

## WAL scope

The first WAL foundation supports logical key-value mutations:

```text
SET key value
DEL key
```

These map to WAL record types:
```text
Set
Del
```

## Sequence number
Each WAL record has a monotonically increasing sequence number.

The initial model only defines and validates the sequence number.

Sequence allocation and persistence are handled by later steps.

## Current non-goals
This stage does not include:
- WAL file format implementation;
- file writing;
- file reading;
- replay;
- checksums;
- fsync policy;
- segment rotation;
- compaction;
- snapshots;
- recovery integration;
- production durability guarantees.
