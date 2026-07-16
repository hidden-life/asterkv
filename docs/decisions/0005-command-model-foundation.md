# ADR 0005: Command Model Foundation
## Status
Accepted.

## Context
AsterKV will support a custom protocol and multiple command execution paths.

The project needs a command representation that is independent from:
- TCP networking;
- wire protocol parsing;
- storage execution;
- future replication and Raft layers.

Without a dedicated command model, protocol parsing and storage execution would become
tightly coupled

## Decision
AsterKV introduces a `command` module.

The initial module provides:
- `CommandType`;
- `CommandRequest`;
- command name conversion;
- expected argument count metadata;
- command request validation.

The initial supported commands are:
- `PING`;
- `SET`;
- `GET`;
- `DEL`;
- `EXISTS`.

## Rationale
This keeps the command lifecycle layered:
```text
protocol bytes
    -> protocol parser
    -> command request
    -> command dispatcher
    -> storage engine
```

The command module defines the middle representation without depending on protocol
or storage.

## Consequences
Future protocol parsing will produce `CommandRequest`.

Future command dispatching will consume `CommandRequest`.

The command module must remain free of networking and storage dependencies.
