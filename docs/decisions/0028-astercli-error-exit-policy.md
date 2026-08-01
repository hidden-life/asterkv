# ADR 0028: astercli Error and Exit-Code Policy
## Status
Accepted.

## Context
`astercli` now renders protocol responses as human-readable output.

The next step is defining how the CLI reports protocol-level errors to users
and shell scripts.

A server-side protocol error is still a valid protocol response, but from a CLI
perspective it represents a failed command.

## Decision
`astercli` uses the following policy:
- successful protocol responses go to stdout and return exit code `0`;
- protocol error responses go to stderr and return exit code `1`;
- TCP/client failures go to stderr and return exit code `1`;
- response parsing/rendering failures go to stderr and return exit code `1`;

In the REPL mode, protocol errors do not stop the session immediately.

The REPL returns exit code `1` if any command failed during the session.

## Rationale
This makes `astercli` predictable in scripts and terminals.

Users can rely on process exit codes.

Protocol errors should not be hidden as successful CLI execution.

## Consequences
Some integration tests must explicitly expect non-zero exit codes for protocol
error responses.

Future work may refine REPL behavior or add a diagnostic mode, but the basic shell-friendly
policy is now fixed.
