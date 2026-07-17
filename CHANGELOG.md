# Changelog
All notable changes to AsterKV will be documented in this file.

AsterKV follows a Semantic Versioning-style release policy using `MAJOR.MINOR.PATCH`
versions and annotated git tags in the `vMAJOR.MINOR.PATCH` format.

## Unreleased
### Added
- Added initial repository structure.
- Added initial standalone binaries:
  - `asterd`
  - `astercli`
  - `asterctl`
- Added initial core version metadata.
- Added project coding conventions.
- Added versioning and release policy documentation.
- Added `AsterKV::Core::Status` for explicit operation status reporting.
- Added `AsterKV::Core::Result<T>` for value-or-error return handling.
- Added GoogleTest-based unit tests for the core status/result model.
- Added development dependency documentation.
- Added the `AsterKV::Command` module.
- Added `CommandType` and `CommandRequest`.
- Added command name conversion and argument count validation.
- Added command model tests without additional external dependencies.
- Added the `AsterKV::Protocol` module.
- Added line-based command tokenization.
- Added command line parsing into `AsterKV::Command::CommandRequest`.
- Added protocol parser tests without additional external dependencies.
- Added protocol parser architecture documentation and ADR.
- Added the `AsterKV::Storage` module.
- Added `StorageEngine` as the storage abstraction.
- Added `InMemoryStorage` with `set`, `get`, `remove`, and `exists` operations.
- Added storage tests without additional external dependencies.
- Added storage foundation architecture documentation and ADR.
- Added the `AsterKV::Execution` module.
- Added `CommandResponse` and command response types.
- Added `CommandDispatcher` for executing command requests against `StorageEngine`.
- Added execution tests without additional external dependencies.
- Added command dispatcher architecture documentation and ADR.
- Added protocol response serialization for command execution results.
- Added serialization for simple string, bulk string, integer, and error responses.
- Added execution result serialization through `serializeExecutionResult()`.
- Added response serialization tests without additional external dependencies.
- Added response serialization architecture documentation and ADR.

### Changed
- Aligned C++ naming conventions:
  - namespaces use PascalCase;
  - functions use lowerCamelCase;
  - variables use lowerCamelCase;
  - headers use `.h`.

### Fixed

### Removed
