# `astercli` REPL UX
`astercli --connect <host:port>` starts TCP REPL mode.

## Startup banner
The REPL prints a short startup banner:
```text
AsterKV TCP REPL
Connected to 127.0.0.1:7721
Type 'help' for commands, 'exit' or 'quit' to leave.
```

## Prompt
The REPL prompt is:
```text
asterkv >
```

## Built-in REPL commands
The following commands are handled by `astercli` and are not sent to the server:
```text
help
?
exit
quit
```

These commands are case-insensitive.

## Help
`help` and `?` print REPL help:
```text
AsterKV REPL commands:
    help, ?         Show this help message
    exit, quit      Exit REPL mode

Server commands:
    PING
    SET <key> <value>
    GET <key>
    DEL <key>
    EXISTS <key>
```

## Empty lines
Empty lines are ignored.

## Exit behavior
`exit`, `quit`, and EOF/Ctrl+D leave REPL mode gracefully.

The REPL prints:
```text
Goodbye.
```

## Error behavior
Protocol errors do not immediately terminate the REPL.

The user can continue typing commands.

If at least one command failed during the session, the REPL process exits with
code `1` after the session ends.

## Current limitations
The REPL does not yet provide:
- persistent TCP session reuse;
- command history;
- autocompletion;
- multiline commands;
- prompt customization;
- client-side command validation.
