# Changelog
All notable changes to AsterKV will be documented in this file.

AsterKV follows a Semantic Versioning-style release policy using `MAJOR.MINOR.PATCH`
versions and annotated git tags in the `vMAJOR.MINOR.PATCH` format.

## Unreleased
### Added
- Started the `0.3.0` storage durability planning and WAL foundation milestone.
- Added the `AsterKV::Wal` module.
- Added WAL record model foundation.
- Added WAL record validation.
- Added WAL record model tests.
- Added storage durability roadmap documentation.
- Added WAL record model architecture documentation and ADR.
- Added WAL record serialization foundation.
- Added WAL record deserialization foundation.
- Added versioned `AKVWAL1` serialized WAL record format.
- Added hex-encoded WAL key/value fields.
- Added WAL serialization roundtrip tests.
- Added WAL serialization architecture documentation and ADR.
- Added WAL file writer foundation.
- Added append-only WAL record file writing.
- Added configurable WAL stream flush-after-write option.
- Added WAL file writer tests.
- Added WAL file writer architecture documentation and ADR.
- Added WAL file reader foundation.
- Added WAL record file reading in file order.
- Added fail-fast handling for invalid WAL record lines.
- Added WAL file reader tests.
- Added WAL file reader architecture documentation and ADR.
- Added WAL replay foundation.
- Added generic `WalReplayTarget`.
- Added WAL record list and WAL file replay helpers.
- Added in-memory WAL replay target.
- Added WAL-backed storage foundation.
- Added WAL-backed storage recovery from WAL file.
- Added WAL replay and WAL-backed storage tests.
- Added WAL replay/storage architecture documentation and ADR.
- Added optional `asterd --wal-file <path>` support.
- Added `wal_file` server config option.
- Added startup recovery from WAL file.
- Connected TCP `SET` and `DEL` mutations to WAL-backed storage when WAL is configured.
- Added WAL recovery integration smoke test.
- Added architecture documentation and ADR for `asterd` WAL file recovery.

### Changed
- Updated project version to `0.3.0` for the next milestone.
- Changed `WalBackedStorage` to implement `StorageEngine`.

## [0.2.0] - 2026-08-01
### Added
- Added `AsterKV::Client` module.
- Added client-side protocol response parsing.
- Added client-side pretty response rendering.
- Added pretty output integration for `astercli`.
- Added human-readable CLI output for local command mode.
- Added human-readable CLI output for TCP single-command mode.
- Added human-readable CLI output for TCP REPL mode.
- Added `astercli` protocol error and exit-code policy.
- Added stderr output for protocol-level CLI errors.
- Added CLI exit code `1` for protocol-level errors.
- Added CLI stdout/stderr/exit-code tests.
- Added REPL-side `help` and `?` commands for `astercli`.
- Added clearer TCP REPL startup banner.
- Added graceful TCP REPL goodbye message.
- Added TCP REPL success smoke test.
- Added CLI response rendering architecture documentation and ADRs.
- Added release documentation for `v0.2.0`.

### Changed
- Updated project version to `0.2.0`.
- Changed `astercli` to print pretty responses by default instead of raw protocol responses.
- Changed `astercli` protocol errors to return non-zero exit codes.
- Changed TCP CLI integration tests to expect non-zero exit codes for protocol errors.
- Improved `astercli` TCP REPL user experience.
- Changed TCP REPL startup text to separate title and endpoint output.

### Known limitations
- `astercli` does not expose a public raw output mode.
- TCP REPL uses one TCP connection per command.
- No command history.
- No autocompletion.
- No persistent storage.
- No WAL.
- No replication.
- No Raft.
- No authentication.
- No TLS.

## [0.1.0] - 2026-07-30
### Added
- Added initial project bootstrap.
- Added project conventions and versioning policy.
- Added core `Status` and `Result<T>` foundations.
- Added command model foundation.
- Added protocol tokenizer and parser foundation.
- Added in-memory storage foundation.
- Added command dispatcher foundation.
- Added protocol response serialization foundation.
- Added local command pipeline.
- Added `astercli --local <command>`.
- Added `asterd --local` mode.
- Added TCP endpoint parsing.
- Added TCP line server foundation.
- Added TCP accept loop with graceful shutdown.
- Added TCP integration smoke tests.
- Added TCP client worker threads.
- Added TCP client worker limits.
- Added TCP idle client timeout.
- Added server configuration file support.
- Added `spdlog`-backed logging facade.
- Added configurable server log level.
- Added TCP client foundation.
- Added `astercli --connect <host:port> <command>`.
- Added TCP REPL mode for `astercli --connect <host:port>`.
- Added TCP client/server integration hardening tests.
- Added release documentation for `v0.1.0`.
- Started the `0.2.0` client UX milestone.
- Added the `AsterKV::Client` module.
- Added protocol response parsing for client-side rendering.
- Added pretty response rendering foundation.
- Added client response rendering tests.
- Added CLI response rendering architecture documentation and ADR.
- Added pretty output integration for `astercli`.
- Added human-readable CLI output for local command mode.
- Added human-readable CLI output for TCP single-command mode.
- Added human-readable CLI output for TCP REPL mode.
- Added documentation and ADR for pretty CLI output by default.
- Added `astercli` protocol error and exit-code policy.
- Added stderr output for protocol-level CLI errors.
- Added CLI exit code `1` for protocol-level errors.
- Added CLI stdout/stderr/exit-code tests.
- Added documentation and ADR for `astercli` error handling.
- Added REPL-side `help` and `?` commands for `astercli`.
- Added clearer TCP REPL startup banner.
- Added graceful TCP REPL goodbye message.
- Added TCP REPL success smoke test.
- Added REPL UX architecture documentation and ADR.

### Changed
- Kept direct `spdlog` usage behind the `AsterKV::Logging` facade.
- Strengthened TCP client/server integration coverage before the first release.
- Updated project version to `0.2.0` for the next milestone.
- Changed `astercli` to print pretty responses by default instead of raw protocol responses.
- Changed `astercli` protocol errors to return non-zero exit codes.
- Changed TCP CLI integration tests to expect non-zero exit codes for protocol errors.
- Improved `astercli` TCP REPL user experience.
- Changed TCP REPL startup text to separate title and endpoint output.

### Known limitations
- Storage is in-memory only.
- No WAL.
- No persistence.
- No replication.
- No Raft.
- No authentication.
- No TLS.
- No production performance claims.

### Fixed

### Removed
