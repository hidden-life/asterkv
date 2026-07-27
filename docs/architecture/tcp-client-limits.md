# TCP Client Lifecycle Limits
AsterKV introduces a basic active worker limit for the TCP server.

## Current option
The TCP server has a maximum active client worker count.

Default:
```text
128
```

`asterd` exposes this option through:
```bash
asterd --listen 127.0.0.1:7721 --max-clients 128
```

## Behavior
When a client connects and the server already has the maximum number of active client
workers, the server sends:
```text
-ERR unavailable maximum client worker limit reached
```

and closes the connection.

Existing clients continue running.

## Why this exists
The current TCP server uses a thread-per-client model.

Without a client limit, a large number of clients could create an unbounded number of threads.

The current limit is a simple lifecycle protection mechanism before worker pools
or event-loop networking are introduced.

## Current limitations
The current implementation does not provide:
- dynamic limit changes;
- per-client identifiers;
- idle timeout policy;
- admission queues;
- worker pool;
- backpressure metrics;
- structured server statistics.

Those capabilities may be added later.
