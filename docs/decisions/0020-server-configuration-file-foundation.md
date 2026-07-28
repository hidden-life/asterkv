# ADR 0020: Server Configuration File Foundation
## Status
Accepted.

## Context
AsterKV has TCP runtime options exposed through CLI flags:
- listen endpoint;
- max client worker count;
- client idle timeout.

CLI flags are useful, but server deployments need a stable configuration file entry point.

The project does not need a full JSON/YAML configuration system yet.

## Decision
AsterKV introduces a simple line-based server configuration format.

The format uses:
```text
key = value
```

The initial supported keys are:
- `listen`;
- `max_clients`;
- `idle_timeout_seconds`.

The `asterd` binary adds:
```bash
asterd --config <path>
```

The config module parses configuration into `Server::TcpServerOptions`.

## Rationale
The line-based format is easy to parse, easy to test, and requires no external
dependencies.

This keeps the server configuration foundation small while still providing a real
configuration-file workflow.

## Consequences
The configration format is intentionally limited.

Future work may introduce:
- inline comments;
- sections;
- environment variable expansion;
- config validation reports;
- config files for storage, logging, WAL, and replication;
- CLI overrides for config-loaded values.
