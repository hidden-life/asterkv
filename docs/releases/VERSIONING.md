# Versioning Policy
AsterKV uses a Semantic Versioning-style version format:
```text
MAJOR.MINOR.PATCH
```

Example versions:
```text
0.1.0
0.1.1
0.2.0
1.0.0
1.1.0
1.1.1
```

## Version meaning
### MAJOR
`MAJOR` is incremented when AsterKV introduces incompatible changes to stable public
interfaces.

This may include incompatible changes to:
- wire protocol;
- storage format;
- cluster behavior;
- administrative API;
- client-facing command behavior.

Before `1.0.0`, compatibility is not guaranteed, but breaking changes must still
be documented.

### MINOR
`MINOR` is incremented for milestone-level functionality.

During the `0.x` development phase, each minor version represents a major project milestone.

Planned milestone direction:
```text
0.1.0       Single-node foundation
0.2.0       Persistence foundation
0.3.0       TTL and command model
0.4.0       Performance foundation
0.5.0       Admin and observability
0.6.0       Replication foundation
0.7.0       Raft foundation
0.8.0       Watch, CAS, and leases
0.9.0       Cluster hardening
1.0.0       First stable release
```

### PATCH
`PATCH` is incremented for fixes and safe improvements within an existing milestone.

Examples:
```text
0.1.1       Fix CLI exit code behavior
0.2.1       Fix WAL recovery edge case
0.3.2       Fix TTL expiration bug
```

## Git tag format
Release tags use the following format:
```text
vMAJOR.MINOR.PATCH
```

Examples:
```text
v0.1.0
v0.1.2
v0.2.0
v1.0.0
```

Tags without the `v` prefix are not used.

## Tag type
AsterKV uses annotated git tags for release.

Example:
```bash
git tag -a v0.1.0 -m "AsterKV v0.1.0"
```

Signed tags may be introduced later when release signing is configured.

## Tag source
Only the `main` branch can be tagged.

Feature branches, fix branches, documentation branches, and release preparation
branches must not be tagged directly.

## What is not a release
The following changes do not automatically create a release:
- repository bootstrap;
- coding style updates;
- documentation-only changes;
- internal refactoring;
- incomplete milestone work;
- experimental implementation branches.

## First release tag
The first planned release tag is:
```text
v0.1.0
```

It will be created only when the single-node foundation is complete.

The `v0.1.0` release requires:
- `asterd` server daemon;
- `astercli` client tool;
- initial TCP server;
- custom protocol foundation;
- in-memory key-value storage;
- commands:
  - `PING`
  - `SET`
  - `GET`
  - `DEL`
  - `EXISTS`
- tests;
- documentation;
- release checklist completion.

## Development versions
The project version in CMake represents the target release line.

Future build metadata may expose additional information:
```text
0.1.0-dev
commit hash
build type
dirty state
```

This is not implemented yet.

Until build metadata support is added, version output remains minimal.
