# ADR 0032: WAL File Writer Foundation

## Status

Accepted.

## Context

AsterKV has a logical WAL record model and a versioned serialized WAL record format.

The next step is writing serialized WAL records to a file.

## Decision

AsterKV adds a foundational append-only WAL file writer.

The writer:

- validates the file path;
- serializes the WAL record;
- opens the target file in append mode;
- writes exactly one serialized record;
- optionally flushes the stream after write.

## Rationale

This keeps the first file writer small and testable.

Opening the file per append is not the final high-performance design, but it avoids lifetime and recovery complexity at this stage.

The writer can later evolve into a long-lived file handle with fsync policy, segment management, and batching.

## Consequences

AsterKV can now write serialized WAL records to files in tests.

This does not yet mean that server storage is durable.

Future work should add:

- WAL file reader;
- replay;
- fsync policy;
- optional `asterd` WAL path;
- recovery validation.
