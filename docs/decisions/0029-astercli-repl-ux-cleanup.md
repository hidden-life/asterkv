# ADR 0029: astercli REPL UX Cleanup
## Status
Accepted.

## Context
`astercli` has TCP REPL mode, pretty output, and shell-friendly protocol error exit
behavior.

The REPL needs basic user-facing ergonomics before the v0.2.0 release.

## Decision
The REPL now provides:
- a clearer startup banner;
- `help` and `?` built-in commands;
- case-insensitive `help`, `?`, `exit`, and `quit`;
- empty-line ignoring;
- graceful EOF/Ctrl+D behavior;
- a consistent goodbye message.

The built-in REPL commands are handled by `astercli` and are not sent to the server.

## Rationale
Basic REPL usability should not require users to know protocol internals.

The REPL should be discoverable and predictable without introducing dependencies
or line-editing libraries.

## Consequences
The REPL is easier to use manually.

The implementation remains simple and does not introduce command history or autocompletion yet.

Future work may add:
- persistent TCP connections;
- line editing;
- history;
- autocompletion;
- command-specific client-side help.
