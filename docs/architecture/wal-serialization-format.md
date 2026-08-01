# WAL Serialization Format

AsterKV uses a versioned text format for serialized WAL records.

## Format

```text
AKVWAL1 <sequence> <type> <key_hex> <value_hex>
```

Each serialized record ends with a newline.

## Examples
Set record:
```text
AKVWAL1 1 set 757365726e616d65 6a61636b736f6e
```

Decoded:
```text
sequence = 1
type     = set
key      = username
value    = jackson
```

Del record:
```text
AKVWAL1 2 del 757365726e616d65 -
```

Decoded:
```text
sequence = 2
type     = del
key      = username
value    = empty
```

## Version fields
The first field is the record format version.

Current version:
```text
AKVWAL1
```

This allows future format changes without guessing how to decode old records.

## Hex fields
Keys and values are hex-encoded.

This avoids ambiguity with:
- spaces;
- separators;
- control characters;
- future binary-safe payloads.

## Empty value marker
The marker:
```text
-
```
represents an empty field.

It is currently used for `del` record values.

## Validation
Deserialization validates both the serialized format and the resulting WAL record
model.

Invalid examples include:
- unsupported format version;
- invalid field count;
- non-numeric sequence number;
- zero sequence number;
- unknown record type;
- invalid hex fields;
- empty key;
- empty value for `set`;
- non-empty value for `del`.

## File writer usage

The WAL file writer appends one serialized WAL record per line.

Example file content:

```text
AKVWAL1 1 set 757365726e616d65 6a61636b736f6e
AKVWAL1 2 del 757365726e616d65 -
```

The serialization format remains independent from the file writer.

## Scope
This step defines serialization and deserialization only.

It does not include:
- WAL file writing;
- WAL file reading;
- reply;
- checksums;
- fsync policy;
- segment rotation;
- recovery integration.
