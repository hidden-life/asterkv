# `astercli` Pretty Output
`astercli` prints human-readable responses by default.

## Motivation
Raw protocol responses are useful for internal protocol validation, but they are not
ideal for user-facing CLI workflows.

Before this step, `astercli` printed responses such as:
```text
+PONG
+OK
$4
alex
:1
-ERR not_found key not found
```

After this step, `astercli` prints:
```text
PONG
OK
alex
1
error: not_found key not found
```

## Scope
Pretty output is enabled for:
- `astercli --local <command>`;
- `astercli --connect <host:port> <command>`;
- `astercli --connect <host:port>` REPL mode.

## Raw protocol
`astercli` does not expose a public raw output mode in this milestone.

Raw protocol behavior remains covered by:
- protocol serializer tests;
- client response parser tests;
- TCP server smoke tests through `nc`.

## Error output
Protocol-level errors are rendered as:
```text
error: <code> <message>
```

Example:
```text
error: not_found key not found
```

At this stage, this step does not change process exit-code policy for protocol-level
errors.

That policy is handled separately.

## Exit code policy
Pretty output is combined with shell-friendly exit-code behavior.

Successful responses are printed to stdout and return exit code `0`.

Protocol errors are printed to stderr and return exit code `1`.

Example:
```bash
astercli --connect 127.0.0.1:7721 GET missing_key
```

Output:
```text
error: not_found key not found
```

Exit code:
```text
1
```
