# ADR 0017: TCP Client Worker Thread Foundation
## Status
Accepted.

## Context
AsterKV had a blocking sequential TCP server.

That server could handle multiple clients over time, but only one active client
at a time.

A long-lived client connection could prevent later clients from being served.

Before introducing a full event loop or worker pool, the project needs a simple
concurrency foundation.

## Decision
AsterKV changes `TcpLineServer::run()` to use a thread-per-client model.

The server:
- accepts client connections in the main server thread;
- starts one worker thread per client;
- keeps accepting later clients while earlier client are still connected;
- joins worker threads during shutdown.

`InMemoryStorage` is protected by a mutex to make the shared in-memory runtime safer
for concurrent command execution.

`CommandDispatcher::dispatch()` and `LocalPipeline::processLine()` are made `const`
to reflect that they do not mutate dispatcher or pipeline state directly.

## Rationale
Thread-per-client is not the final networking model, but it is a useful increment step.

It validates:
- shared runtime state across concurrent clients;
- storage synchronization;
- server shutdown with active clients;
- process-level TCP integration under concurrent load.

## Consequences
The server can now response to a second client while another client remains connected.

The implementation may create many threads if many clients connect.

Future work should introduce:
- client limits;
- worker pools;
- event-loop based I/O;
- more advanced lifecycle management.


