# ADR 0002: Coding and Repository Conventions
## Status
Accepted.

## Context
AsterKV is intended to be developed as a professional systems software project.

The repository must remain understandable for maintainers and future contributors.
Naming, branching, documentation, and dependency rules must be explicit from the
beginning.

## Decision
AsterKV uses the following conventions:
- C++ namespaces use PascalCase, for example `AsterKV::Core`.
- C++ functions and methods use lowerCamelCase.
- C++ variables use lowerCamelCase.
- C++ headers use the `.h` extension.
- Every development step starts from a dedicated git branch.
- Documentation is updated as part of every step.
- Dependencies are introduced only when required by implemented functionality.

## Rationale
These rules keeps the project consistent and reduce ambiguity for future contributors.

The project intentionally avoids speculative dependencies and speculative architecture.
New modules, libraries, and abstractions are added only when they are needed by the current
implementation step.

## Consequences
Existing boostrap code must be aligned with these conventions before adding new
functionality.

Future code reviews should reject changes that introduce inconsistent naming,
undocumented behavior changes, or unnecessary dependencies.
