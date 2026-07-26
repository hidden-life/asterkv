# Local Pipeline Foundation
AsterKV introduces a local in-process command pipeline.

The local pipeline connects the already implemented layers:
```text
text command
    -> protocol parser
    -> command request
    -> command dispatcher
    -> storage engine
    -> command response
    -> protocol response serializer
    -> text response
```

## Current component
The pipeline module currently provides:
- `AsterKV::Pipeline::LocalPipeline`

## Current behavior
`LocalPipeline::processLine()` accepts a single textual command line and returns
a serialized protocol response.

Examples:
```text
PING
```
returns:
```text
+PONG\r\n
```

```text
SET username alex
```
returns:
```text
+OK\r\n
```

```text
GET username
```
returns:
```text
$7\r\nalex\r\n
```

## Responsibilities
The local pipeline is responsible for orchestrating existing layers.

It does not implement:
- protocol parsing logic;
- command execution logic;
- response serialization logic;
- storage behavior.

Those responsibilities remain in their own modules.

## Non-goals
The current local pipeline does not implement:
- TCP networking;
- sessions;
- authentication;
- pipelining;
- requestIDs;
- concurrency;
- persistence.

Those capabilities will be added later.
