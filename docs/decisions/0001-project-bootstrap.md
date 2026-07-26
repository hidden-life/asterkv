# ADR 0001: Project Bootstrap
## Status
Accepted

## Context
AsterKV is intended to become a distributed high-performance key-value database
written in C++23.

The project must support multiple standalone binaries instead of a single
overloaded command. This follows the operational model of systems such as etcd,
where server, admin, client, benchmark, and recovery tooling have different
responsibilities.

## Decision
The initial repository contains:
- `asterd` for the server daemon;
- `astercli` for user-facing client commands;
- `asterctl` for administrative commands;
- `asterkv_core` as the first internal library;
- CMake presets for debug and release builds;
- CTest smoke tests.

No external dependencies are introduced in the bootstrap step.

## Rationale
This keeps the first step minimal, buildable, and testable.

External dependencies such as GoogleTest, GoogleBenchmark, fmt, spdlog, CLI11
and nlohmann/json will be added only when the code actually needs them.

## Consequences
The initial binaries only support `--help` and `--version`.

Real server, protocol, storage, and network functionality will be added in
later steps.
