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
- graceful shutdown through `SIGTERM`;
- concurrent client handling while another client remains connected;
- active client worker limit rejection;
- idle client timeout and worker slot release.

## Netcat behavior
The project currently uses:
```bash
nc -q 1
```

This asks netcat to close the connection shortly after stdin reaches EOF.

This is important because the current server serves one connected client until
that client disconnects.

## Concurrent client check
The smoke test starts one long-lived TCP client and keeps its connection open
briefly.

While that client is still connected, the test sends `PING` from another client.

The second client must received `+PONG`.

This verifies that the server is no longer sequential at the client-serving level.

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

## Client limit smoke test
The client limit smoke test starts `asterd` with:
```bash
--max-clients 1
```

The test keeps one TCP client connected and then opens another client.

The second client must receive:
```text
-ERR unavailable maximum client worker limit reached
```

This verifies that the active worker limit is enforced.

## Idle timeout smoke test
The idle timeout smoke test starts `asterd` with:
```bash
--max-clients 1 --idle-timeout 1
```

The test connects one client, sends a command, and then keeps the connection idle.

A second client is initially rejected because the active client worker limit is reached.

After the idle timeout releases the first worker slot, a later client can connect successfully.

## astercli TCP smoke test
The `astercli_tcp_smoke_test` starts `asterd` and uses `astercli --connect` to verify:
- `PING`;
- `SET`;
- `GET`;
- `EXISTS`;
- `DEL`;
- missing key response;
- graceful server shutdown.

The test validates the real client-server workflow without using `nc` as the command client.

## Current limitations
The smoke test uses a fixed local test port:
```text
127.0.0.1:17721
```

If that port is already in use, the test may fail.

Future improvements may add dynamic test port allocation.
