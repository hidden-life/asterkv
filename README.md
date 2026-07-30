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
printf 'PING\n' | nc -q 1 127.0.0.1 7721
```

Expected response:
```text
+PONG
```

Send multiple commands over one connection:
```bash
printf 'SET username alex\nGET username\n' | nc -q 1 127.0.0.1 7721
```

Expected response:
```text
+OK
$4
alex
```

Limit active TCP client workers:
```bash
./build/debug/apps/asterd/asterd --listen 127.0.0.1:7721 --max-clients 128
```

When the limit is reached, new clients receive an unavailable error response and
the connection is closed.

Configure idle client timeout:
```bash
./build/debug/apps/asterd/asterd --listen 127.0.0.1:7721 --idle-timeout 300
```

Use both TCP lifecycle options:
```bash
./build/debug/apps/asterd/asterd --listen 127.0.0.1:7721 --max-clients 128 --idle-timeout 300
```

Idle clients are disconnected after the configured timeout.

Run with debug logging:
```bash
./build/debug/apps/asterd/asterd --listen 127.0.0.1:7721 --log-level debug
```

Stop the server:
```text
Ctrl+C
```

The server prints:
```text
AsterKV server stopped.
```

The examples use `nc -q 1` so netcat closes the client connection after stdin reaches EOF.
This matters because the current TCP server is sequential and serves one connected
client at a time.

### Server configuration file
Create a config file:
```ini
listen = 127.0.0.1:7721
max_clients = 128
idle_timeout_seconds = 300
log_level = info
```

Run `asterd` with the config file:
```bash
./build/debug/apps/asterd/asterd --config config/asterd.conf
```

### Server logging
`asterd --listen` initializes the default server logger.

Example log line:
```text
[2026-01-01 12.00.00.000] [info] [asterd] starting TCP server runtime
```

AsterKV uses `spdlog` behind the `AsterKV::Logging` facade.

Runtime code should use the facade instead of calling `spdlog` directly.

### TCP client
Start the server:
```bash
./build/debug/apps/asterd/asterd --listen 127.0.0.1:7721
```

Run commands through `astercli`:
```bash
./build/debug/apps/astercli/astercli --connect 127.0.0.1:7721 PING
./build/debug/apps/astercli/astercli --connect 127.0.0.1:7721 SET username alex
./build/debug/apps/astercli/astercli --connect 127.0.0.1:7721 GET username
```

`astercli` currently prints raw protocol responses.

## Development principles
- C++23.
- Linux-first.
- CMake-based.
- No Boost.
- No RocksDB.
- Dependencies are introduced only when they become necessary.
- No phantom modules, files, helpers, or APIs.
- Documentation is updated after every development step.
