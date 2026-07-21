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
$7
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
$7
alex
> quit
```

Local mode does not use TCP networking and does not persist data between separate
processes.

## Development principles
- C++23.
- Linux-first.
- CMake-based.
- No Boost.
- No RocksDB.
- Dependencies are introduced only when they become necessary.
- No phantom modules, files, helpers, or APIs.
- Documentation is updated after every development step.
