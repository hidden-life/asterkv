# Response Serialization Foundation
AsterKV introduces protocol response serialization for command execution results.

The serializer converts internal execution responses into textual protocol responses.

## Current response format
The initial response format is line-oriented and uses CRLF line endings.

### Simple string
```text
+OK\r\n
+PONG\r\n
```

### Bulk string
```text
$5\r\n
value\r\n
```
Serialized as one byte stream:
```text
$5\r\nvalue\r\n
```

### Integer
```text
:1\r\n
:0\r\n
```

### Error
```text
-ERR not_found key not found\r\n
```

## Current API
The protocol module provides:
- `serializeCommandResponse()`;
- `serializeStatus()`;
- `serializeExecutionResult()`.

## Design boundaries
The serializer does not execute commands.

The serializer does not access storage.

The serializer does not parse protocol input.

It only converts existing internal results into protocol output bytes.

## Future work
Future protocol work may introduce:
- request identifiers;
- binary-safe frames;
- pipelining;
- streaming responses;
- protocol version negotiation.
