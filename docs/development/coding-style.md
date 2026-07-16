# Coding Style
This document defines the coding conventions used in AsterKV.

## Language
AsterKV uses C++23.

Compiler extensions are disabled. Code should remain portable across modern Linux
C++ toolchains.

## Naming
### Namespaces
Namespaces use PascalCase.
```c++
namespace AsterKV::Core {

}
```

### Types
Types use PascalCase.
```c++
struct Version final {

};
```

### Function and methods
Functions and methods use lowerCamelCase.
```c++
std::string_view versionString() noexcept;

void printUsage(std::string_view execName);
```

### Variables
Variables use lowerCamelCase.
```c++
const std::string_view executableName = argv[0];
```

### Headers
Public and private C++ headers use the `.h` extension.
```text
version.h
```

The `.hpp` extension is not used in this project.

## Error handling
The project will prefer explicit status/result for recoverable errors.

Exceptions are not planned for hot-path control flow. Module-specific decisions
must be documented before implementation.

## Ownership
Use RAII and explicit ownership.

Avoid global mutable state.

Prefer constructor injection for required dependencies.

## Includes
Headers should include what they use.

Source files should include their own header first when applicable.

## Documentation requirement
Every development ste must update documentation when it changes:
- public behavior;
- architecture;
- repository workflow;
- protocol;
- storage format;
- operational behavior;
- developer workflow.
