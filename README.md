# AsterKV
AsterKV is distributed high-performance key-value database written in modern C++.

The long-term goal is to combine Redis-like speed with etcd-like reliability:
- fast key-value operations;
- custom network protocol;
- custom persistence engine;
- WAL and snapshots;
- replication;
- Raft-based cluster mode;
- watch, lease, and compare-and-swap features.

## Repository
`github.com/hidden-life/asterkv`

## Initial binaries
| Binary | Purpose |
| --- | ---|
| `asterd` | AsterKV server daemon |
| `astercli` | User-facing command-line client |
| `asterctl` | Administrative control tool |

Feature tools will be added only when needed.

## Build
```bash
cmake --preset debug
cmake --build --preset debug
```

## Test
```bash
ctest --preset debug
```

## Run
```bash
./build/debug/apps/asterd/asterd --version
./build/debug/apps/astercli/astercli --version
./build/debug/apps/asterctl/asterctl --help
```

## Local CLI mode
Run a single command locally:
```bash
./build/debug/apps/astercli/astercli local PING
./build/debug/apps/astercli/astercli local "SET username alex"
```

## Server local stdin
Run one command through the server binary without TCP:
```bash
./build/debug/apps/asterd/asterd --local PING
```

Run multiple commands through stdin:
```bash
printf 'SET username alex\nGET username\n' | ./build/debug/apps/asterd/asterd --local
```

Example output:
```text
+OK
$4
alex
```

Server local mode uses in-memory storage and does not persist data between processes.

Start an interactive in-memory local session:
```bash
./build/debug/apps/astercli/astercli local
```

Example:
```text
> SET username alex
+OK
> GET username
$4
alex
> quit
```

Local mode does not use TCP networking and does not persist data between separate
processes.

## TCP server foundation
Start the TCP server:
```bash
./build/debug/apps/asterd/asterd --listen 127.0.0.1:7721
```

Send a command from another terminal:
```bash
printf 'PING\n' | nc 127.0.0.1 7721
```

Expected response:
```text
+PONG
```

Send multiple commands over one connection:
```bash
printf 'SET username alex\nGET username\n' | nc 127.0.0.1 7721
```

Expected response:
```text
+OK
$4
alex
```

The current TCP server accepts clients in a blocking loop and serves one client at a time.
It keeps running until Ctrl+C is pressed.

Stop the server:
```text
Ctrl+C
```

The server prints:
```text
AsterKV server stopped.
```

## Development principles
- C++23.
- Linux-first.
- CMake-based.
- No Boost.
- No RocksDB.
- Dependencies are introduced only when they become necessary.
- No phantom modules, files, helpers, or APIs.
- Documentation is updated after every development step.
