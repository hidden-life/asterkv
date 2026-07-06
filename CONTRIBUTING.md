# Contributing to AsterKV
AsterKV is a distributed key-value database written in modern C++.

The project is developed as a professional systems software project. Contributions
should be small, reviewable, documented, and covered by tests when behavior changes.

## Development model
Every development step must follow this sequence:
1. Create a dedicated git branch.
2. Implement the smallest useful change.
3. Update documentation.
4. Build the project.
5. Run tests.
6. Review the diff.
7. Commit using a conventional commit message.

## Branch naming
Use short and descriptive branch names:
```text
feature/<scope>
fix/<scope>
docs/<scope>
refactor/<scope>
test/<scope>
```

Examples:
```text
feature/project-conventions
feature/in-memory-storage
fix/protocol-invalid-command
docs/storage-format
```

## Commit format
AsterKV uses conventional commits:
```text
<type>(<scope>): <description>
```

Examples:
```text
feat(project): bootstrap repository structure
refactor(core): align naming conventions
docs(development): add coding style guide
test(storage): cover missing key lookup
```

Common types:
```text
feat
fix
docs
test
refactor
build
ci
chore
perf
```

## C++23 style
AsterKV uses C++23.

Required naming conventions:

| Entity | Style | Example |
|---|---|---|
| Namespace | PascalCase | `AsterKV::Core` |
| Type | PascalCase | `Version` |
| Function | lowerCamelCase | `versionString()` |
| Method | lowerCamelCase | `applyEntity()` |
| Variable | lowerCamelCase | `executableName` |
| Constant | lowerCamelCase for now | `defaultClientPort` |
| Header file | `.h` | `version.h` |

## Dependencies
Dependencies are added only when the are required by an implementation feature.

Do not introduce libraries speculatively.

Current bootstrap stage intentionally avoids external dependencies.

## Documentation
Documentation is part of the implementation.

A change is incomplete if it changes behavior, architecture, workflow, storage format,
or operational behavior without updating the related documentation.

## Versioning and releases
AsterKV uses `MAJOR.MINOR.PATCH` versions and annotated release tags in the
`vMAJOR.MINOR.PATCH` format.

Examples:
```text
v0.1.0
v0.1.1
v0.2.0
```

Not every merge to `main` is release.

Release tags are created only from `main`.

Documentation-only, conventions-only, bootstrap, and incomplete milestone changes
do not receive release tags.

See:
- [docs/releases/VERSIONING.md](docs/releases/VERSIONING.md)
- [docs/releases/release-process.md](docs/releases/release-process.md)

## Tests
Behavioral change must include tests.

Bootstrap smoke tests are currently implemented through CTest. GoogleTest will be
introduced when unit-level testing becomes necessary.
