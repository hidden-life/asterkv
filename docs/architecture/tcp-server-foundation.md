# TCP Server Socket Foundation
AsterKV introduces a minimal TCP server socket foundation.

The server accepts line-based commands over TCP and executes them through the local
command pipeline.

## Usage
Start the server:
```bash
asterd --listen 127.0.0.1:7721
```

Send a command:
```bash
printf 'PING\n' | nc 127.0.0.1 7721
```

Expected response:
```text
+PONG
```

## Current behavior
The initial TCP server:
- binds to an IPv4 host and port;
- listens for one client;
- accepts one client connection;
- reads newline-delimited commands;
- processes commands through `LocalPipeline`;
- writes serialized protocol responses;
- exits after the client disconnects.

## Current protocol
Commands are line-based:
```text
SET username alex
GET username
```

Responses use the existing protocol serializer:
```text
+OK
$4
alex
```

## Design boundary
The TCP server does not parse command semantics directly.

It delegates to:
```text
TCP bytes
    -> line reader
    -> LocalPipeline
    -> protocol response bytes
```

The local pipeline remains responsible for protocol parsing, command dispatching,
storage access, and response serialization.

## Current limitations
The current TCP server does not provide:
- multiple client loop;
- concurrency;
- event loop;
- graceful shutdown;
- socket timeouts;
- TLS;
- authentication;
- binary-safe framing;
- request pipelining semantics beyond sequential newline processing;
- persistence.

These capabilities will be introduced incrementally.
