# TCP Client/Server Integration Hardening
Step 0.25 hardens the TCP client/server workflow before the first release.

## Scope
This step does not add a new runtime feature.

It strengthens existing functionality through additional integration smoke tests.

## Covered paths
The integration test suite now covers:
- raw TCP server smoke testing through `nc`;
- TCP client worker limits;
- TCP idle client timeout;
- `astercli --connect` single-command mode;
- `astercli --connect` REPL mode;
- `asterd --config` with real TCP client commands;
- protocol error responses through `astercli`;
- server health after protocol errors.

## Config-based server startup
`asterd_config_tcp_smoke_test` starts the server through:
```bash
asterd --config <temporary-config-file>
```

The test verifies that the server uses:
```ini
listen = 127.0.0.1:<port>
max_clients = 8
idle_timeout_seconds = 10
log_level = warn
```

Then it sends commands through `astercli --connect`.

## Protocol error hardening
`astercli_tcp_error_smoke_test` verifies that protocol-level command errors do not
stop the server.

Examples:
```text
NOPE key
GET missing_key
```

Expected responses:
```text
-ERR invalid_argument unknown command
-ERR not_found key not found
```

The test than sends valid commands to confirm that the server is still healthy.

## Current limitations
The integration tests are still smoke tests.

They do not yet cover:
- high concurrency;
- long-running soak testing;
- packet fragmentation;
- malformed binary input;
- persistent client sessions;
- authentication;
- TLS.
