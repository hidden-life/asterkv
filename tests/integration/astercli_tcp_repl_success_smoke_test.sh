#!/usr/bin/env sh

set -eu

ASTERD_BIN="$1"
ASTERCLI_BIN="$2"
TIMEOUT_BIN="$3"
HOST="$4"
PORT="$5"

SERVER_PID=""
SERVER_LOG="$(mktemp "${TMPDIR:-/tmp}/asterkv_astercli_tcp_repl_success_server.XXXXXX.log")"

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

run_single_cli() {
    "${TIMEOUT_BIN}" 5 "${ASTERCLI_BIN}" --connect "${HOST}:${PORT}" "$@"
}

run_repl_cli_success() {
    set +e
    output="$(
        printf '%s\n' \
            "" \
            "help" \
            "PING" \
            "SET repl_success_key repl_success_value" \
            "GET repl_success_key" \
            "EXISTS repl_success_key" \
            "QuIt" |
            "${TIMEOUT_BIN}" 5 "${ASTERCLI_BIN}" --connect "${HOST}:${PORT}" 2>&1
    )"
    status="$?"
    set -e

    if [ "${status}" -ne 0 ]; then
        echo "Expected successful REPL session to return zero" >&2
        echo "Exit status: ${status}" >&2
        echo "Output:" >&2
        printf '%s\n' "${output}" >&2
        fail "expected successful REPL exit code"
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

        response="$(run_single_cli PING 2>/dev/null || true)"

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

response="$(run_repl_cli_success)"

assert_contains "${response}" "AsterKV TCP REPL" "REPL startup title"
assert_contains "${response}" "Connected to ${HOST}:${PORT}" "REPL startup endpoint"
assert_contains "${response}" "Type 'help' for commands" "REPL startup help hint"
assert_contains "${response}" "AsterKV REPL commands:" "REPL help title"
assert_contains "${response}" "Server commands:" "REPL help server commands"
assert_contains "${response}" "PONG" "REPL PING"
assert_contains "${response}" "OK" "REPL SET"
assert_contains "${response}" "repl_success_value" "REPL GET"
assert_contains "${response}" "1" "REPL EXISTS"
assert_contains "${response}" "Goodbye." "REPL goodbye"

kill -TERM "${SERVER_PID}" 2>/dev/null || fail "failed to send SIGTERM to server"

SERVER_EXIT_CODE=0
wait "${SERVER_PID}" || SERVER_EXIT_CODE="$?"
SERVER_PID=""

if [ "${SERVER_EXIT_CODE}" -ne 0 ]; then
    fail "server did not stop gracefully after SIGTERM; exit code ${SERVER_EXIT_CODE}"
fi

exit 0
