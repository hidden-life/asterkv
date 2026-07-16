# Storage Foundation
AsterKV introduces a storage module as the first execution layer for key-value operations.

The initial implementation is in-memory only.

## Current components
The storage module currently provides:
- `AsterKV::Storage::StorageEngine`
- `AsterKV::Storage::InMemoryStorage`

## StorageEngine
`StorageEngine` is the abstract interface for key-value operations.

Current operations:

| Method | Purpose |
| --- | --- |
| `set()` | Store or overwrite a key-value pair |
| `get()` | Read a value by key |
| `remove()` | Delete a key |
| `exists()` | Check whether a key exists |

## InMemoryStorage
`InMemoryStorage` stores values in memory using a hash map;

It is intended for:
- early command execution;
- protocol integration tests;
- future standalone server foundation;
- baseline behavior before WAL and persistence.

## Error handling
Storage operations use the core status/result model.

Examples:
- empty keys return `invalidArgument`;
- missing key return `NotFound`;
- successful write/delete operations return `Status::ok()`;
- successful read operations return `Result<std::string>`;
- successful existence checks return `Result<bool>`.

## Non-goals
The current storage module does not implement:
- WAL;
- snapshots;
- persistence;
- TTL;
- concurrency control;
- replication;
- transactions;
- Raft state machine integration.

Those features will be introduced in later steps.
