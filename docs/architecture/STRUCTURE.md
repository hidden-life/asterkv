# Project Structure
AsterKV is organized as a modular C++23 project.

## Top-level layout
```text
apps/       Executable applications.
cmake/      Reusable CMake modules.
docs/       Project documentation.
include/    Public headers.
src/        Source code for internal and public libraries.
```

## Current applications
```text
apps/asterd     Server daemon.
apps/astercli   User-facing CLI.
apps/asterctl   Administrative control tool.
```

## Current libraries
```text
src/core        Core project-level utilities.
src/command     Command model and validation.
src/protocol    Protocol tokenizer/parser foundation.
```

The core library currently contains:
- version metadata;
- status/result error handling primitives.

The command library currently contains:
- command type definitions;
- command request representation;
- command name conversion;
- command argument count validation.

The protocol library currently contains:
- line-based command tokenizer;
- command line parser;
- conversion from protocol text to `CommandRequest`.

## Planned future modules
The following modules are planned but not created yet:
```text
net             Linux networking and event loop.
protocol        Custom AsterKV wire protocol.
command         Command parsing and dispatching.
storage         In-memory and persistent storage engines.
wal             Write-ahead log.
snapshot        Snapshot writer and loader.
replication     Leader/follower replication.
raft            Raft consensus.
config          Configuration loading and validation.
```

They will be introduced step by step.
