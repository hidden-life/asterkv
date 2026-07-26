#!/usr/bin/env sh

set -eu

ASTERD_BIN="$1"
NC_BIN="$2"
TIMEOUT_BIN="$3"
HOST="$4"
PORT="$5"

SERVER_PID=""
SERVER_LOG="$(mktemp "${TMPDIR:-/tmp}/asterkv_tcp_server.XXXXXX.log")"
LONG_CLIENT_OUTPUT="$(mktemp "${TMPDIR:-/tmp}/asterkv_tcp_long_client.XXXXXX.out")"

cleanup() {
    if [ -n "${SERVER_PID}" ] && kill -0 "${SERVER_PID}" 2>/dev/null; then
        kill -TERM "${SERVER_PID}" 2>/dev/null || true
        wait "${SERVER_PID}" 2>/dev/null || true
    fi

    rm -f "${SERVER_LOG}" "${LONG_CLIENT_OUTPUT}"
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

run_client() {
    input="$1"

    printf '%b' "${input}" | "${TIMEOUT_BIN}" 5 "${NC_BIN}" -q 1 "${HOST}" "${PORT}"
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

        response="$(run_client 'PING\n' 2>/dev/null || true)"

        if printf '%s' "${response}" | grep -F -- "+PONG" >/dev/null; then
            return 0
        fi

        attempt=$((attempt + 1))
        sleep 0.1
    done

    fail "server did not start accepting TCP connections"
}

"${ASTERD_BIN}" --listen "${HOST}:${PORT}" >"${SERVER_LOG}" 2>&1 &
SERVER_PID="$!"

wait_for_server

response="$(run_client 'SET username jackson\nGET username\n')"
assert_contains "${response}" "+OK" "SET over TCP"
assert_contains "${response}" "jackson" "GET over same TCP connection"

response="$(run_client 'GET username\n')"
assert_contains "${response}" "jackson" "shared state across TCP clients"

response="$(run_client 'EXISTS username\n')"
assert_contains "${response}" ":1" "EXISTS over TCP"

response="$(run_client 'DEL username\nEXISTS username\n')"
assert_contains "${response}" ":1" "DEL over TCP"
assert_contains "${response}" ":0" "EXISTS after DEL over TCP"

response="$(run_client 'GET username\n')"
assert_contains "${response}" "-ERR not_found key not found" "missing key after DEL over TCP"

(
    {
        printf 'SET long_client active\n'
        sleep 5
    } | "${TIMEOUT_BIN}" 8 "${NC_BIN}" -q 1 "${HOST}" "${PORT}" >"${LONG_CLIENT_OUTPUT}"
) &
LONG_CLIENT_PID="$!"

sleep 0.5

response="$(
    printf 'PING\n' | "${TIMEOUT_BIN}" 2 "${NC_BIN}" -q 1 "${HOST}" "${PORT}" || true
)"

assert_contains "${response}" "+PONG" "concurrent PING while another client remains connected"

wait "${LONG_CLIENT_PID}" || fail "long-lived client failed"

longClientResponse="$(cat "${LONG_CLIENT_OUTPUT}")"
assert_contains "${longClientResponse}" "+OK" "long-lived client SET response"

response="$(run_client 'GET long_client\n')"
assert_contains "${response}" "active" "state written by long-lived client"

kill -TERM "${SERVER_PID}" 2>/dev/null || fail "failed to send SIGTERM to server"

SERVER_EXIT_CODE=0
wait "${SERVER_PID}" || SERVER_EXIT_CODE="$?"
SERVER_PID=""

if [ "${SERVER_EXIT_CODE}" -ne 0 ]; then
    fail "server did not stop gracefully after SIGTERM; exit code ${SERVER_EXIT_CODE}"
fi

exit 0
