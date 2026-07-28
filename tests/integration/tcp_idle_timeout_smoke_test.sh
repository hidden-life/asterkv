#!/usr/bin/env sh

set -eu

ASTERD_BIN="$1"
NC_BIN="$2"
TIMEOUT_BIN="$3"
HOST="$4"
PORT="$5"

SERVER_PID=""
IDLE_CLIENT_PID=""
SERVER_LOG="$(mktemp "${TMPDIR:-/tmp}/asterkv_tcp_idle_server.XXXXXX.log")"
IDLE_CLIENT_OUTPUT="$(mktemp "${TMPDIR:-/tmp}/asterkv_tcp_idle_client.XXXXXX.out")"

cleanup() {
    if [ -n "${IDLE_CLIENT_PID}" ] && kill -0 "${IDLE_CLIENT_PID}" 2>/dev/null; then
        kill -TERM "${IDLE_CLIENT_PID}" 2>/dev/null || true
        wait "${IDLE_CLIENT_PID}" 2>/dev/null || true
    fi

    if [ -n "${SERVER_PID}" ] && kill -0 "${SERVER_PID}" 2>/dev/null; then
        kill -TERM "${SERVER_PID}" 2>/dev/null || true
        wait "${SERVER_PID}" 2>/dev/null || true
    fi

    rm -f "${SERVER_LOG}" "${IDLE_CLIENT_OUTPUT}"
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

wait_for_idle_client_response() {
    attempt=0

    while [ "${attempt}" -lt 50 ]; do
        if grep -F -- "+OK" "${IDLE_CLIENT_OUTPUT}" >/dev/null 2>&1; then
            return 0
        fi

        if ! kill -0 "${IDLE_CLIENT_PID}" 2>/dev/null; then
            fail "idle client exited before timeout check"
        fi

        attempt=$((attempt + 1))
        sleep 0.1
    done

    fail "idle client did not receive initial response"
}

wait_until_limit_slot_is_released() {
    attempt=0

    while [ "${attempt}" -lt 50 ]; do
        response="$(run_client 'GET idle_key\n' 2>/dev/null || true)"

        if printf '%s' "${response}" | grep -F -- "active" >/dev/null; then
            return 0
        fi

        attempt=$((attempt + 1))
        sleep 0.1
    done

    fail "idle client worker slot was not released"
}

"${ASTERD_BIN}" \
    --listen "${HOST}:${PORT}" \
    --max-clients 1 \
    --idle-timeout 1 \
    >"${SERVER_LOG}" 2>&1 &
SERVER_PID="$!"

wait_for_server

(
    {
        printf 'SET idle_key active\n'
        sleep 5
    } | "${TIMEOUT_BIN}" 8 "${NC_BIN}" -q 1 "${HOST}" "${PORT}" >"${IDLE_CLIENT_OUTPUT}"
) &
IDLE_CLIENT_PID="$!"

wait_for_idle_client_response

response="$(run_client 'PING\n' || true)"
assert_contains "${response}" "-ERR unavailable maximum client worker limit reached" "client limit while idle client is connected"

wait_until_limit_slot_is_released

wait "${IDLE_CLIENT_PID}" 2>/dev/null || true
IDLE_CLIENT_PID=""

idleClientResponse="$(cat "${IDLE_CLIENT_OUTPUT}")"
assert_contains "${idleClientResponse}" "+OK" "idle client initial SET response"

kill -TERM "${SERVER_PID}" 2>/dev/null || fail "failed to send SIGTERM to server"

SERVER_EXIT_CODE=0
wait "${SERVER_PID}" || SERVER_EXIT_CODE="$?"
SERVER_PID=""

if [ "${SERVER_EXIT_CODE}" -ne 0 ]; then
    fail "server did not stop gracefully after SIGTERM; exit code ${SERVER_EXIT_CODE}"
fi

exit 0
