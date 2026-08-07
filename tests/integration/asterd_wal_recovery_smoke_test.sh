#!/usr/bin/env sh

set -eu

ASTERD_BIN="$1"
ASTERCLI_BIN="$2"
TIMEOUT_BIN="$3"
HOST="$4"
PORT="$5"

SERVER_PID=""
SERVER_LOG="$(mktemp "${TMPDIR:-/tmp}/asterkv_wal_recovery_server.XXXXXX.log")"
WAL_FILE="$(mktemp "${TMPDIR:-/tmp}/asterkv_wal_recovery.XXXXXX.wal")"

cleanup() {
    if [ -n "${SERVER_PID}" ] && kill -0 "${SERVER_PID}" 2>/dev/null; then
        kill -TERM "${SERVER_PID}" 2>/dev/null || true
        wait "${SERVER_PID}" 2>/dev/null || true
    fi

    rm -f "${SERVER_LOG}" "${WAL_FILE}"
}

trap cleanup EXIT INT TERM

fail() {
    echo "ERROR: $*" >&2

    if [ -f "${WAL_FILE}" ]; then
        echo "---- WAL file ----" >&2
        cat "${WAL_FILE}" >&2 || true
        echo "------------------" >&2
    fi

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

start_server() {
    : >"${SERVER_LOG}"

    "${ASTERD_BIN}" \
        --listen "${HOST}:${PORT}" \
        --max-clients 8 \
        --idle-timeout 10 \
        --log-level warn \
        --wal-file "${WAL_FILE}" \
        --wal-sync "fsync_every_write" \
        >"${SERVER_LOG}" 2>&1 &

    SERVER_PID="$!"

    wait_for_server
}

stop_server() {
    kill -TERM "${SERVER_PID}" 2>/dev/null || fail "failed to send SIGTERM to server"

    SERVER_EXIT_CODE=0
    wait "${SERVER_PID}" || SERVER_EXIT_CODE="$?"
    SERVER_PID=""

    if [ "${SERVER_EXIT_CODE}" -ne 0 ]; then
        fail "server did not stop gracefully after SIGTERM; exit code ${SERVER_EXIT_CODE}"
    fi
}

start_server

response="$(run_cli SET username alex)"
assert_contains "${response}" "OK" "SET username"

response="$(run_cli GET username)"
assert_contains "${response}" "alex" "GET username before restart"

stop_server

if ! grep -F -- "616c6578" "${WAL_FILE}" >/dev/null; then
    fail "WAL file does not contain hex-encoded value alex"
fi

start_server

response="$(run_cli GET username)"
assert_contains "${response}" "alex" "GET username after restart"

response="$(run_cli EXISTS username)"
assert_contains "${response}" "1" "EXISTS username after restart"

response="$(run_cli DEL username)"
assert_contains "${response}" "1" "DEL username after restart"

stop_server

start_server

set +e
missing_response="$(run_cli GET username 2>&1)"
missing_status="$?"
set -e

if [ "${missing_status}" -eq 0 ]; then
    echo "Expected GET deleted username to fail after WAL replay" >&2
    printf '%s\n' "${missing_response}" >&2
    fail "GET deleted username unexpectedly succeeded"
fi

assert_contains "${missing_response}" "error: not_found key not found" "GET deleted username after restart"

stop_server

exit 0
