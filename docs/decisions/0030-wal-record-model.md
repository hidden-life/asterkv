# ADR 0030: WAL Record Model Foundation

## Status

Accepted.

## Context

AsterKV currently stores data in memory only.

The next milestone is storage durability planning and WAL foundation.

Before implementing file I/O, AsterKV needs a stable logical record model for mutations that can later be serialized, written, read, and replayed.

## Decision

AsterKV adds a new `AsterKV::Wal` module.

The first public API defines:

- `WalSequenceNumber`;
- `WalRecordType`;
- `WalRecord`;
- `makeSetRecord`;
- `makeDelRecord`;
- `validateWalRecord`;
- `walRecordTypeToString`.

The initial record types are:

- `Set`;
- `Del`.

## Rationale

The initial storage command set mutates state through `SET` and `DEL`.

A logical WAL model keeps persistence independent from the current command parser and protocol layer.

Starting with the record model avoids committing to a file format too early.

## Consequences

Future steps can add:

- serialization;
- file writer;
- file reader;
- replay;
- optional `asterd` WAL path.

This decision does not claim that AsterKV is durable yet.
