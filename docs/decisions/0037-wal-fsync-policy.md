# ADR 0037: WAL fsync Policy Foundation

## Status

Accepted.

## Context

AsterKV WAL writer now owns a long-lived append handle.

The next durability hardening step is adding an explicit sync policy.

The previous implementation used C++ stream flush behavior, which is not equivalent to operating-system-level durability.

## Decision

AsterKV adds `WalSyncPolicy` with three policies:

- `None`;
- `FsyncOnFlush`;
- `FsyncEveryWrite`.

`FsyncEveryWrite` is the default.

The WAL writer uses Linux file descriptors and calls `fsync()` according to the selected policy.

`asterd` exposes the policy through:

- CLI: `--wal-sync <policy>`;
- config: `wal_sync = <policy>`.

## Rationale

The policy makes durability/performance tradeoffs explicit.

Using Linux file descriptors prepares the writer for future work:

- directory sync;
- segment rotation;
- checksums;
- better recovery diagnostics.

## Consequences

AsterKV has a stronger WAL persistence foundation.

This still does not claim production-grade durability because recovery integrity
and corruption handling are not complete.
