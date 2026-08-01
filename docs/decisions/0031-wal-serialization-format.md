# ADR 0031: WAL Serialization Format

## Status

Accepted.

## Context

AsterKV has a logical WAL record model with `Set` and `Del` records.

The next step is a stable serialization format that can later be written to files, read back, and replayed.

A naive whitespace-separated format such as `1 SET key value` is easy to read but becomes ambiguous when keys or values contain spaces or separators.

## Decision

AsterKV uses a versioned text format:

```text
AKVWAL1 <sequence> <type> <key_hex> <value_hex>
```

Keys and values are hex-encoded.

An empty value is represented by `-`.

## Rationale

The format is:
- simple to inspect;
- deterministic;
- versioned;
- unambiguous for spaces and separators;
- easy to parse in tests;
- suitable for file-based WAL records in later steps.

Hex encoding is less compact than binary encoding, but it keeps the initial WAL foundation simple and debuggable.

## Consequences

Future WAL file writer and reader steps can operate on one serialized record per line.

Future versions may add:
- checksums;
- timestamps;
- compression;
- segment metadata;
- binary record encoding.

This decision does not introduce durable storage by itself.
