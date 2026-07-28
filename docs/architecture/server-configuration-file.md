# Server Configuration File
AsterKV supports a simple line-based server configuration file.

## Example
```ini
# AsterKV server configuration example.

listen = 127.0.0.1:7721
max_clients = 128
idle_timeout_seconds = 300
```

## Running with config
```bash
./build/debug/apps/asterd/asterd --config config/asterd.conf
```

## Format
The current format intentionally simple:
```text
key = value
```

Blank lines are ignored.

Lines starting with `#` are ignored.

Inline comments are not supported yet.

## Supported keys
```text
listen                      TCP listen endpoint in host:port format.
max_clients                 Maximum active TCP client workers.
idle_timeout_seconds        TCP client idle timeout in seconds.
```

## Defaults
All keys are optional.

Missing keys use built-in defaults:
```text
listen = 127.0.0.1:7721
max_clients = 128
idle_timeout_seconds = 300
```

## Current limitations
The current config format does not support:
- inline comments;
- includes;
- environment variables expansion;
- sections;
- arrays;
- nested objects;
- hot reload;
- CLI overrides for `--config`.

Those capabilities may be introduced later.
