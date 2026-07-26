# Server Local Stdin Mode
AsterKV introduces a local stdin mode for `asterd`.

This mode runs the server-side command pipeline without TCP networking.

## Usage
Execute one command locally:
```bash
asterd --local PING
asterd --local "SET username alex"
```

Process commands from stdin:
```bash
printf "SET username alex\nGET username" | asterd --local
```

## Behavior
`asterd --local <command>` creates a temporary in-memory storage instance, executes
 one command, prints the protocol response, and exits.

`asterd --local` reads command lines from stdin until EOF.

The stdin mode keeps one in-memory storage instance alive for the lifetime of the process.

## Examples
Input:
```text
SET username alex
GET username
```

Output:
```text
+OK
$7
alex
```

## Design boundary
Local stdin mode is not a TCP server.

It is a server-side entry point for exercising the command pipeline before networking
is introduced.

It uses:
- `InMemoryStorage`;
- `LocalPipeline`;
- protocol parser;
- command dispatcher;
- response serializer.

## Current limitations
Local stdin mode does not provide:
- TCP sockets;
- concurrent clients;
- persistence;
- WAL;
- authentication;
- sessions;
- request IDs;
- graceful network shutdown.

These capabilities will be introduced later.
