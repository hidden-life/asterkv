# Command Model
AsterKV has a dedicated command model layer.

The command model is responsible for representing supported database commands
independently from networking, protocol parsing, and storage execution.

## Current commands
The initial command set is:

| Command | Arguments | Purpose |
| --- | ---: | --- |
| `PING` | 0 | Health check command |
| `SET` | 2 | Store a key-value pair |
| `GET` | 1 | Read a key |
| `DEL` | 1 | Delete a key |
| `EXISTS` | 1 | Check whether a key exists |

## Types
The command module currently provides:
- `AsterKV::Command::CommandType`
- `AsterKV::Command::CommandRequest`
- `commandTypeFromString()`
- `commandTypeToString()`
- `expectedArgumentCount()`
- `validateCommandRequest()`

## Design boundaries
The command model does not parse the wire protocol.

The command model does not execute commands.

The command model does not access storage.

Those responsibilities belong to future modules:
- protocol parser
- command dispatcher
- storage engine

## Error handling
Command parsing and validation use the core status/result model:
- `AsterKV::Core::Result<T>`
- `AsterKV::Core::Status`

Invalid command names and invalid argument counts return `ErrorCode::InvalidArgument`.
