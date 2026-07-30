# ADR 0026:  CLI Response Rendering Foundation
## Status
Accepted.

## Context
AsterKV v0.1.0 introduced a runnable single-node client/server foundation.

`astercli` currently prints raw protocol responses.

That is useful for validating the protocol, but it is not ideal for user-facing
CLI workflows.

## Decision
AsterKV adds a new `AsterKV::Client` module.

The module provides:
- protocol response parsing;
- pretty response rendering;
- a foundation for future `astercli` UX improvements.

This step does not change `astercli` behavior yet.

## Rationale
Rendering should not be embedded directly inside `apps/astercli/main.cpp`.

A client module allows the parser and renderer to be tested independently.

It also keeps future client UX features reusable across single-command and REPL modes.

## Consequences

Future steps can integrate pretty output into `astercli`.

The raw protocol output path can still be preserved through an explicit raw mode.

Future work may add:

- `astercli --raw`;
- `astercli --pretty`;
- response colors;
- command-specific rendering;
- better error display;
- response-based exit-code policy.
