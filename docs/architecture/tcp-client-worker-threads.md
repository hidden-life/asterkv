# TCP Client Worker Thread Foundation
AsterKV introduces a thread-per-client TCP serving model.

## Previous model
The previous TCP server was sequential:
```text
accept client
    -> serve client until disconnect
    -> accept next client
```

That model was simple, but one long-lived client could block all later clients.

## Current model
The TCP server now accepts a client and starts a worker thread for it:
```text
accept client
    -> start client worker
    -> continue accepting clients
```

Each worker handles one TCP client connection.

## Shared storage
All workers use the same server runtime storage.

`InMemoryStorage` is now protected by a mutex, so concurrent `SET`, `GET`, `DEL`,
and  `EXISTS` call are serialized at the storage boundary.

## Shutdown behavior
Client sockets use short receive/send timeouts.

This allows worker threads to periodically check the server stop callback and exit
during shutdown.

On shutdown, the accept loop stops and joins client worker threads.

## Client limit
The thread-per-client model is bounded by a maximum active worker count.

When the limit is reached, new clients receive:
```text
-ERR unavailable maximum client worker limit reached
```
and connection is closed.

This prevents unbounded thread creation.

## Current limitations
The current worker model is intentionally simple.

It does not provide:
- worker pool;
- max client limit;
- backpressure;
- per-client logging;
- idle connection policy;
- advance cancellation;
- non-blocking I/O;
- event loop.

Those capabilities will be introduced later.
