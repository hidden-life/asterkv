# Development Dependencies
AsterKV introduces external dependencies only when they are required by implemented
functionality.

## Require base tools
For local development on Debian:
```bash
sudo apt update
sudo apt install -y build-essential cmake ninja-build
```

## Test dependencies
GoogleTest is required for unit tests.
```bash
sudo apt install -y libgtest-dev
```

AsterKV uses CMake package discovery for GoogleTest:
```cmake
find_package(GTest CONFIG REQUIRED)
```

## Dependency policy
Do not add dependencies speculatively.

A dependency is accepted only when:
- a current implementation step requires it;
- the dependency is documented;
- the build integration is explicit;
- the dependency does not replace a core subsystem that AsterKV intends to implement
in-house.

AsterKV does not use:
- Boost;
- RocksDB.
