# CLI Response Rendering Foundation
AsterKV 0.2.0 starts with a client-side response rendering foundation.

## Goal
The TCP server currently returns raw protocol response.

Examples:
```text
+PONG
+OK
$4
alex
:1
-ERR not_found key not found
```

`astercli` currently prints these raw responses directly.

The response rendering foundation adds a client-side parser and pretty renderer
that can later be used by `astercli`.

## Module
The rendering foundation lives in:
```text
AsterKV::Client
```

Current public API:
```c++
parseProtocolResponse(...)
renderPrettyResponse(...)
renderPrettyResponseText(...)
```

## Supported response kinds
The parser supports the response kinds currently produced by the AsterKV protocol
serializer:
- simple string;
- error;
- integer;
- bulk string.

## Pretty rendering
Pretty rendering converts raw protocol responses into user-facing CLI output.

Examples:
```text
+PONG                   -> PONG
+OK                     -> OK
$4 + payload alex       -> alex
:1                      -> 1
-ERR not_found key...   -> error: not_found key not found
```

## Integration status
`astercli` now uses the client response renderer for default user-facing output.

Pretty rendering is used by:
- local command mode;
- single-command TCP mode;
- TCP REPL mode.

## Current limitations
The renderer does not yet support:
- explicit diagnostic raw protocol output mode;
- response colors;
- tables;
- arrays;
- maps;
- multiline pretty formatting;
- command-specific formatting;
- client-side exit-code policy for protocol errors.
