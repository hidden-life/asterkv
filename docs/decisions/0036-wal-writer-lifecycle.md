# ADR 0036: WAL Writer Lifecycle and Append Handle

## Status

Accepted.

## Context

The initial WAL writer opened the WAL file for every appended record.

That design was simple and testable, but it is not suitable for the next durability hardening steps.

Future work needs a writer lifecycle that can support:

- explicit flush;
- fsync policy;
- batching;
- segment rotation;
- metrics;
- better error reporting.

## Decision

AsterKV changes `WalFileWriter` to own a long-lived append stream.

The writer supports:

- `open`;
- `appendRecord`;
- `flush`;
- `close`;
- `isOpen`.

`WalBackedStorage` uses a long-lived writer instance.

The existing `appendWalRecordToFile` helper remains for tests and simple one-shot usage.

## Consequences

The WAL writer lifecycle is now explicit.

The implementation is ready for fsync policy in the next step.

This decision does not introduce fsync or production-grade durability guarantees.
