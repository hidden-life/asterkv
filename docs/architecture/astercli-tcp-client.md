# `astercli` TCP Client Foundation
`astercli` can execute a single command against a running `asterd` TCP server.

## Usage
```bash
astercli --connect <host:port> [command]
```

Example:
```bash
./build/debug/apps/astercli/astercli --connect 127.0.0.1:7721 PING
```

## Client-server workflow
Start server:
```bash
./build/debug/apps/asterd/asterd --listen 127.0.0.1:7721
```

Send commands:
```bash
./build/debug/apps/astercli/astercli --connect 127.0.0.1:7721 SET username alex
./build/debug/apps/astercli/astercli --connect 127.0.0.1:7721 GET username
```

## Protocol output
At this stage, `astercli` prints raw protocol responses.

Examples:
```text
+PONG
+OK
$4
alex
:1
-ERR not_found key not found
```

Pretty response rendering is intentionally deferred.

## Connection model
For each command, `astercli`:
1. opens a TCP connection;
2. sends one line-based command;
3. shuts down the write side;
4. reads the server response until the connection closes;
5. prints the raw response.

This simple model is enough for single-command CLI execution.

## REPL mode
If no command is provided, `astercli` starts TCP REPL mode:
```bash
astercli --connect 127.0.0.1:7721
```

At this stage, REPL mode still uses one TCP connection per command.

## Current limitations
The current TCP client does not provide:
- connection reuse;
- authentication;
- TLS;
- response pretty-printing;
- custom client timeout CLI options;
- persistent REPL connections;
- command history;
- autocompletion.
