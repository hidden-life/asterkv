# Command Dispatcher Foundation
AsterKV introduces an execution module that dispatches validated command requests
to the storage layer.

The dispatcher connects:
```text
CommandRequest
    -> CommandDispatcher
    -> StorageEngine
    -> CommandResponse
```

## Current components
The execution module currently provides:
- `AsterKV::Execution::CommandResponse`
- `AsterKV::Execution::CommandDispatcher`

## Command responses
Current response types:

| Type | Purpose |
| --- |---------|
| `SimpleString` | Simple textual success response |
| `BulkString` | Value response for reads |
| `Integer` | Numeric response for boolean/count-like results |

## Current command behavior

| Command | Response |
| --- | --- |
| `PING` | `SimpleString("PONG")` |
| `SET key value` | `SimpleString("OK")` |
| `GET key` | `BulkString(value)` or `NotFound` |
| `DEL key` | `Integer(1)` when deleted, `Integer(0)` when missing |
| `EXISTS key` | `Integer(1)` when present, `Integer(0)` when missing |

## Design boundaries
The dispatcher does not parse protocol text.

The dispatcher does not format wire protocol responses.

The dispatcher does not own storage.

The dispatcher receives a `StorageEngine` reference and executes commands against it.

## Future work
Future steps will add:
- response serialization;
- CLI integration;
- TCP server integration;
- WAL-backed storage;
- TTL-aware execution.
