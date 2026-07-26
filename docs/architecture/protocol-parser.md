# Protocol Parser Foundation
AsterKV currently uses a minimal line-based protocol parser.

The parser converts a textual command line into the internal command model:
```text
SET username alex
```
becomes:
```text
CommandRequest {
    type = Set,
    arguments = ["username", "alex"]
}
```

## Current scope
The initial parser supports whitespace-separated command lines.

Supported commands are defined by the command module:

| Command | Arguments |
| --- |----------:|
| `PING` |         0 |
| `SET` |         2 |
| `GET` |         1 |
| `DEL` |        1  |
| `EXISTS` | 1 |

## Responsibilities
The protocol parser is responsible for:
- tokenizing a command line;
- resolving a command name;
- building `AsterKV::Command::CommandRequest`;
- validating command argument count through the command module.

## Non-goals
The current parser does not implement:
- TCP networking;
- binary-safe frames;
- quoted strings;
- escaped values;
- streaming;
- pipelining;
- request identifiers;
- RESP compatibility.

Those capabilities belong to future protocol versions.

## Design boundary
The parser depends on the command module and the core status/result model.

It does not depend on storage, networking, WAL, replication, or Raft.
