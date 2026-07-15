# Core Status and Result Model
AsterKV uses explicit status/result objects for recoverable errors.

This model is introduced before networking, protocol parsing, storage, WAL, and replication
so these modules can share one consistent error-handling approach.

## Status
`AsterKV::Core::Status` represents the result of an operation that does not return a value.

Examples:
```c++
return AsterKV::Core::Status::ok();
return AsterKV::Core::Status::invalidArgument("key must not be empty");
return AsterKV::Core::Status::notFound("key not found");
```

## Result
`AsterKV::Core::Result<T>` represents either a successful value or an error status.

Examples:
```c++
return AsterKV::Core::Result<std::string>::success("value");
return AsterKV::Core::Result<std::string>::failure(
    AsterKV::Core::Status::notFound("key not found")
);
```

## Design rules
Operations without a return value should return `Status`.

Operations with a return value should return `Result<T>`.

`Result<void>` is intentionally not supported. Use `Status` instead.

`Result<Status>` is intentionally not supported. Use `Status` directly.

## Error codes
Current error codes:
```text
ok
unknown
invalid_argument
not_found
already_exists
conflict
unavailable
internal
```

These codes are intentionally generic. Module-specific errors can be added later
when the implementation requires them.

## Exceptions
Recoverable errors should not use exceptions in hot-path code.

The status/result model is intended for expected operational errors such as:
- invalid command input
- missing keys
- unavailable dependency
- protocol errors
- storage errors
- internal consistency errors
