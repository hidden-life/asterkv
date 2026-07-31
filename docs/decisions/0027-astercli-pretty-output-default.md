# ADR 0027: astercli Pretty Output by Default
## Status
Accepted.

## Context
AsterKV v0.1.0 exposed raw protocol responses through `astercli`.

AsterKV 0.2.0 focuses on CLI response rendering and client UX.

A response rendering foundation was added before this step.

## Decision
`astercli` now uses `AsterKV::Client::renderPrettyResponseText` for user-facing
output.

Pretty output is the default for:
- local command mode;
- single-command TCP mode;
- TCP REPL mode.

No public `--raw` mode is added at this stage.

## Rationale
The CLI should be user-facing and should not require users to understand the wire protocol.

The raw protocol is still validated by lower-level protocol and integration tests.

Deferring a raw output switch keeps the public CLI smaller and avoids exposing internal protocol
details too early.

## Consequences
Users see cleaner responses:
```text
PONG
OK
alex
1
error: not_found key not found
```

Future work may add explicit debug or diagnostic modes if raw output becomes necessary
for automation or protocol troubleshooting.
