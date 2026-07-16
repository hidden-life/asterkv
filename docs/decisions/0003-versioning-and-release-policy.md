# ADR 0003: Versioning and Release Policy
## Status
Accepted.

## Context
AsterKV is a systems software project that will include a custom network protocol, custom persistence format, command-line tools, and eventually distributed cluster behavior.

Versioning must be explicit from the beginning because future changes may affect:
- client compatibility;
- server compatibility;
- storage format compatibility;
- cluster compatibility;
- operational procedures.

## Decision
AsterKV uses a Semantic Versioning-style format:
```text
MAJOR.MINOR.PATCH
```

Release git tags use the following format:
```text
vMAJOR.MINOR.PATCH
```

Examples:
```text
v0.1.0
v0.1.1
v0.2.0
v1.0.0
```

Release tags must be annotated.

Only the `main` branch can be tagged.

During the `0.x` development phase, minor versions represent milestone releases.

Patch versions represent fixes inside an existing milestone.

## Initial milestone direction
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

## Rationale
The `vMAJOR.MINOR.PATCH` tag format is common, easy to read, and works well with
GitHub Releases and release automation.

Annotated tags are preferred because they contain tag metadata and provide a clearer
release history.

Restricting release tags to `main` keeps the release process predictable.

## Consequence
Not every merge to `main` creates a release.

Bootstrap, conventions, documentation, and incomplete feature work do not receive
release tags.

The first planned release tag is `v0.1.0`, create only after the single-node foundation
is complete.
