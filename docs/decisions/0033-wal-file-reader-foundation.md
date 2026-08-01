# ADR 0033: WAL File Reader Foundation

## Status

Accepted.

## Context

AsterKV has:

- a WAL record model;
- a versioned serialized WAL record format;
- an append-only WAL file writer.

The next step is reading serialized records back from a file.

## Decision

AsterKV adds a foundational WAL file reader.

The reader reads one serialized WAL record per line and deserializes each line through the WAL record codec.

An empty WAL file is valid.

An empty line inside a WAL file is invalid.

If any record line is invalid, reading the file fails.

## Rationale

Fail-fast behavior is simpler and safer for the first reader foundation.

Silent skipping of invalid lines could hide corruption.

Returning records in file order prepares the next step: replay into in-memory storage.

## Consequences

AsterKV can now roundtrip WAL records through files in tests:

```text
WalRecord -> serialize -> append file -> read file -> deserialize
```

This does not yet mean that server storage is durable.

Future work should add:
- replay;
- recovery policy;
- optional asterd WAL path;
- fsync policy;
- corruption diagnostics.
