# CLI Local Mode
AsterKV introduces a local execution mode in `astercli`.

Local mode executes commands through the in-process pipeline without TCP networking.

## Usage
Run the command:
```bash
astercli local PING
astercli local "SET username alex"
astercli local "GET username"
```

Start an interactive local REPL:
```bash
astercli local
```

Exit the REPL:
```text
quit
exit
```

## Behavior
`astercli local <command>` creates a temporary in-memory storage instance, executes
one command, prints the protocol response, and exits.

`astercli local` start an interactive REPL. The REPL keeps one in-memory storage
instance alive for the lifetime of the process.

## Examples
```text
> PING
+PONG
```

```text
SET username alex
+OK
```

```text
GET username
$7
alex
```

## Current limitations
Local mode does not provide:
- TCP networking;
- persistence;
- shared state between separate CLI invocations;
- authentication;
- command history;
- line editing;
- concurrent sessions.

These features will be introduces later.
