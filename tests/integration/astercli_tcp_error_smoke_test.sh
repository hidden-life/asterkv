#!/usr/bin/env sh

set -eu

ASTERD_BIN="$1"
ASTERCLI_BIN="$2"
TIMEOUT_BIN="$3"
HOST="$4"
PORT="$5"

SERVER_PID=""
SERVER_LOG="$(mktemp "${TMPDIR:-/tmp}/asterkv_astercli_tcp_error_server.XXXXXX.log")"

cleanup() {
    if [ -n "${SERVER_PID}" ] && kill -0 "${SERVER_PID}" 2>/dev/null; then
        kill -TERM "${SERVER_PID}" 2>/dev/null || true
        wait "${SERVER_PID}" 2>/dev/null || true
    fi

    rm -f "${SERVER_LOG}"
}

trap cleanup EXIT INT TERM

fail() {
    echo "ERROR: $*" >&2

    if [ -f "${SERVER_LOG}" ]; then
        echo "---- server log ----" >&2
        cat "${SERVER_LOG}" >&2
        echo "--------------------" >&2
    fi

    exit 1
}

run_cli() {
    "${TIMEOUT_BIN}" 5 "${ASTERCLI_BIN}" --connect "${HOST}:${PORT}" "$@"
}

run_cli_expect_failure() {
    set +e
    output="$("${TIMEOUT_BIN}" 5 "${ASTERCLI_BIN}" --connect "${HOST}:${PORT}" "$@" 2>&1)"
    status="$?"
    set -e

    if [ "${status}" -eq 0 ]; then
        echo "Expected astercli command to fail but it succeeded: $*" >&2
        echo "Output:" >&2
        printf '%s\n' "${output}" >&2
        fail "expected command failure"
    fi

    printf '%s' "${output}"
}

assert_contains() {
    haystack="$1"
    needle="$2"
    description="$3"

    if ! printf '%s' "${haystack}" | grep -F -- "${needle}" >/dev/null; then
        echo "Unexpected response for ${description}" >&2
        echo "Expected to contain: ${needle}" >&2
        echo "Actual response:" >&2
        printf '%s\n' "${haystack}" >&2
        fail "${description} failed"
    fi
}

wait_for_server() {
    attempt=0

    while [ "${attempt}" -lt 50 ]; do
        if ! kill -0 "${SERVER_PID}" 2>/dev/null; then
            fail "server process exited before accepting connections"
        fi

        response="$(run_cli PING 2>/dev/null || true)"

        if printf '%s' "${response}" | grep -F -- "PONG" >/dev/null; then
            return 0
        fi

        attempt=$((attempt + 1))
        sleep 0.1
    done

    fail "server did not start accepting TCP connections"
}

"${ASTERD_BIN}" \
    --listen "${HOST}:${PORT}" \
    --max-clients 8 \
    --idle-timeout 10 \
    --log-level warn \
    >"${SERVER_LOG}" 2>&1 &
SERVER_PID="$!"

wait_for_server

response="$(run_cli_expect_failure NOPE key)"
assert_contains "${response}" "error: invalid_argument unknown command" "unknown command"

response="$(run_cli_expect_failure GET missing_key)"
assert_contains "${response}" "error: not_found key not found" "missing key"

response="$(run_cli SET healthy_key healthy_value)"
assert_contains "${response}" "OK" "SET after protocol errors"

response="$(run_cli GET healthy_key)"
assert_contains "${response}" "healthy_value" "GET after protocol errors"

response="$(run_cli PING)"
assert_contains "${response}" "PONG" "PING after protocol errors"

kill -TERM "${SERVER_PID}" 2>/dev/null || fail "failed to send SIGTERM to server"

SERVER_EXIT_CODE=0
wait "${SERVER_PID}" || SERVER_EXIT_CODE="$?"
SERVER_PID=""

if [ "${SERVER_EXIT_CODE}" -ne 0 ]; then
    fail "server did not stop gracefully after SIGTERM; exit code ${SERVER_EXIT_CODE}"
fi

exit 0
