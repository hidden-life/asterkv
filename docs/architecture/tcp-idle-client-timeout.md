# TCP Idle Client Timeout
AsterKV introduces an idle timeout for TCP client connections.

## Current option
The TCP server has a client idle timeout in seconds.

Default:
```text
300
```

`asterd` exposes this option through:
```bash
asterd --listen 127.0.0.1:7721 --idle-timeout 300
```

## Behavior
A TCP client is considered idle when no bytes are received from that client for the
configured timeout.

When the timeout is reached, the server closes the client connection.

This releases the worker thread and frees one active client slot.

## Why this exists
The TCP server currently uses a thread-per-client model.

Without an idle timeout, a client can connect and remain silent indefinitely, keeping a worker
thread alive.

The idle timeout is a simple lifecycle protection mechanism.

## Current limitations
The current idle timeout:
- is configured globally for the server;
- is measured per client worker;
- does not send a protocol error before closing the connection;
- does not expose timeout metrics.

Future work may add structured connection lifecycle events and metrics.
