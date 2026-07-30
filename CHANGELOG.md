# Changelog
All notable changes to AsterKV will be documented in this file.

AsterKV follows a Semantic Versioning-style release policy using `MAJOR.MINOR.PATCH`
versions and annotated git tags in the `vMAJOR.MINOR.PATCH` format.

## Unreleased
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

### Changed
- Kept direct `spdlog` usage behind the `AsterKV::Logging` facade.
- Strengthened TCP client/server integration coverage before the first release.
- Updated project version to `0.2.0` for the next milestone.

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
