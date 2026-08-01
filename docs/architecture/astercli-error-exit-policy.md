# `astercli` Error and Exit-Code Policy
`astercli` distinguishes successful responses, protocol errors, and client/runtime
failures.

## Successful protocol responses
Successful protocol responses are printed to stdout.

Examples:
```text
PONG
OK
alex
1
```

Exit code:
```text
0
```

## Protocol errors
Protocol-level errors are returned by the server as valid protocol responses.

`astercli` renders them as human-readable errors and prints them to stderr.

Example:
```text
error: not_found key not found
```

Exit code:
```text
1
```

## TCP/client failures
TCP connection failures, endpoint parsing failures, response paring failures, and
rendering failures are printed to stderr.

Exit code:
```text
1
```

## REPL behavior
In TCP REPL mode, protocol errors do not stop the session.

The REPL continues accepting commands.

If any command in the REPL session fails, the REPL process exits with code `1`
after the user exits the session.

Built-in REPL commands such as `help`, `?`, `exit`, and `quit` do not count as protocol
failures because they are handled locally by `astercli`.

## Rationale
This policy makes `astercli` usable in shell scripts.

Successful commands can be chained normally.

Protocol errors are visible through process exit codes.
