# TCP Integration Smoke Tests
AsterKV includes a TCP integration smoke test for the blocking sequential TCP server.

The smoke test starts `asterd --listen` in the background, connects through `nc`,
sends commands, verifies responses, and then stops the server with `SIGTERM`.

## Current test
The integration test verifies:
- TCP server startup;
- `PING` over TCP;
- `SET` and `GET` over TCP connection;
- shared in-memory state across multiple TCP clients;
- `EXISTS`;
- `DEL`;
- missing-key error response;
- graceful shutdown through `SIGTERM`.

## Netcat behavior
The project currently uses:
```bash
nc -q 1
```

This asks netcat to close the connection shortly after stdin reaches EOF.

This is important because the current server serves one connected client until
that client disconnects.

## Test dependencies
The TCP integration smoke test requires:
- `/bin/sh`;
- `nc`;
- `timeout`.

If `nc` or `timeout` is not available, the test is skipped unless strict test
dependency mode is enabled.

Strict mode:
```bash
cmake --preset debug -DASTERKV_REQUIRE_TEST_DEPENDENCIES=ON
```

## Current limitations
The smoke test uses a fixed local test port:
```text
127.0.0.1:17721
```

If that port is already in use, the test may fail.

Future improvements may add dynamic test port allocation.
