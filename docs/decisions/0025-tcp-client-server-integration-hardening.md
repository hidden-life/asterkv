# ADR 0025: TCP Client/Server Integration Hardening
## Status
Accepted.

## Context
AsterKV now has:
- TCP server mode;
- config file support;
- configurable log level;
- `astercli` single-command TCP mode;
- `astercli` TCP REPL mode.

Before preparing `v0.1.0`, the project needs stronger end-to-end checks around
these paths.

## Decision
AsterKV adds integration smoke tests for:
- config-based server startup with `astercli`;
- protocol errors through `astercli`;
- server health after protocol errors.

## Rationale
The first release should be based on a validated client/server workflow, not only
unit-level command and parser tests.

Using `astercli` in integration tests validates the actual user-facing command path.

Testing protocol errors ensures that command-level failures remain normal responses
and do not terminate the server.

## Consequences
The integration test suite better reflects the expected `v0.1.0` usage model.

Future hardening may add:
- concurrency stress tests;
- soak tests;
- protocol fuzzing;
- persistent client-session tests;
- response rendering tests.
