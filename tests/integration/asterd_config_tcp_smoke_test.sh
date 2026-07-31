#!/usr/bin/env sh

set -eu

ASTERD_BIN="$1"
ASTERCLI_BIN="$2"
TIMEOUT_BIN="$3"
HOST="$4"
PORT="$5"

SERVER_PID=""
SERVER_LOG="$(mktemp "${TMPDIR:-/tmp}/asterkv_config_tcp_server.XXXXXX.log")"
CONFIG_FILE="$(mktemp "${TMPDIR:-/tmp}/asterkv_config_tcp.XXXXXX.conf")"

cleanup() {
    if [ -n "${SERVER_PID}" ] && kill -0 "${SERVER_PID}" 2>/dev/null; then
        kill -TERM "${SERVER_PID}" 2>/dev/null || true
        wait "${SERVER_PID}" 2>/dev/null || true
    fi

    rm -f "${SERVER_LOG}" "${CONFIG_FILE}"
}

trap cleanup EXIT INT TERM

fail() {
    echo "ERROR: $*" >&2

    if [ -f "${CONFIG_FILE}" ]; then
        echo "---- config ----" >&2
        cat "${CONFIG_FILE}" >&2
        echo "----------------" >&2
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

cat >"${CONFIG_FILE}" <<EOF
listen = ${HOST}:${PORT}
max_clients = 8
idle_timeout_seconds = 10
log_level = warn
EOF

"${ASTERD_BIN}" \
    --config "${CONFIG_FILE}" \
    >"${SERVER_LOG}" 2>&1 &
SERVER_PID="$!"

wait_for_server

if ! grep -F -- "AsterKV listening on ${HOST}:${PORT}" "${SERVER_LOG}" >/dev/null; then
    fail "server did not use listen address from config"
fi

if ! grep -F -- "Max client workers: 8" "${SERVER_LOG}" >/dev/null; then
    fail "server did not use max_clients from config"
fi

if ! grep -F -- "Client idle timeout seconds: 10" "${SERVER_LOG}" >/dev/null; then
    fail "server did not use idle_timeout_seconds from config"
fi

if ! grep -F -- "Log level: warn" "${SERVER_LOG}" >/dev/null; then
    fail "server did not use log_level from config"
fi

response="$(run_cli SET config_key config_value)"
assert_contains "${response}" "OK" "SET config_key"

response="$(run_cli GET config_key)"
assert_contains "${response}" "config_value" "GET config_key"

response="$(run_cli EXISTS config_key)"
assert_contains "${response}" "1" "EXISTS config_key"

kill -TERM "${SERVER_PID}" 2>/dev/null || fail "failed to send SIGTERM to server"

SERVER_EXIT_CODE=0
wait "${SERVER_PID}" || SERVER_EXIT_CODE="$?"
SERVER_PID=""

if [ "${SERVER_EXIT_CODE}" -ne 0 ]; then
    fail "server did not stop gracefully after SIGTERM; exit code ${SERVER_EXIT_CODE}"
fi

exit 0
