# `astercli` TCP REPL Foundation
`astercli` can run an interactive TCP REPL against a running `asterd` server.

## Usage
Start `asterd`:
```bash
./build/debug/apps/asterd/asterd --listen 127.0.0.1:7721
```

Start REPL:
```bash
./build/debug/apps/astercli/astercli --connect 127.0.0.1:7721
```

Example session:
```text
AsterKV TCP REPL connected to 127.0.0.1:7721
Type 'exit' or 'quit' to leave.
asterkv > PING
PONG
asterkv > SET username alex
OK
asterkv > GET username
alex
asterkv > quit
```

## Exit commands
The REPL exits on:
```text
exit
quit
```

The commands are case-insensitive.

## Empty lines
Empty lines are ignored.

## Protocol output
The REPL prints human-readable responses by default.

Pretty response rendering is intentionally deferred.

## Connection model
The REPL currently uses `Network::TcpLineClient` for each command.

That means every entered command:
1. opens a TCP connection;
2. sends one line-based command;
3. shuts down the write side;
4. reads the response;
5. closes the connection.

This is intentionally simple and matches the current TCP client foundation.

## Current limitations
The current REPL does not provide:
- persistent TCP connection reuse;
- multiline commands;
- command history;
- autocompletion;
- pretty response rendering;
- client-side command parsing;
- authentication;
- TLS.
