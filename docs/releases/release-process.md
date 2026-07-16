# Release Process
This document describes the release process for AsterKV.

## Release principles
A release is created only from the `main` branch.

A release requires:
- complete milestone scope;
- updated documentation;
- updated changelog;
- successful debug build;
- successful release build;
- successful tests;
- reviewed git history;
- annotated git tag.

## Release checklist
Before creating a release tag:
```bash
git checkout main
git pull --ff-only origin main
```

Verify repository state:
```bash
git status
```

Expected result:
```text
nothing to commit, working tree clean
```

Run debug build:
```bash
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

Run release build:
```bash
cmake --preset release
cmake --build --preset release
```

Verify version output:
```bash
./build/release/apps/asterd/asterd --version
./build/release/apps/astercli/astercli --version
./build/release/apps/asterctl/asterctl --version
```

Review changelog:
```bash
git diff
```

## Creating a release tag
Create an annotated tag:
```bash
git tag -a v0.1.0 -m "AsterKV v0.1.0"
```

Push the tag:
```bash
git push origin v0.1.0
```

## GitHub release
After pushing a release tag, create a GitHub Release using the matching tag.

The release notes must be based on `CHANGELOG.md`.

## Patch releases
Patch releases use the same process.

Example:
```bash
git tag -a v0.1.1 -m "AsterKV v0.1.1"
git push origin v0.1.1
```

Patch releases should include only fixes, documentation corrections, or safe maintenance
changes.

## Pre-release versions
Pre-release tags may be introduced later.

Examples:
```text
v1.0.0-alpha.1
v1.0.0-beta.1
v1.0.0-rc.1
```

Pre-release tags are not required during the early bootstrap phase.

## Version bump policy
Version bumps happen as part of release preparation.

AsterKV does not bump versions for every feature branch.

After a release, the project may move toward the next development version once build metadata
support is implemented.
