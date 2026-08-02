# WAL-backed Storage Foundation
AsterKV provides a foundational WAL-backed storage layer for in-memory storage.

## Scope
The WAL-backed storage layer writes a WAL record before applying a mutation to storage.

Supported mutations:
```text
SET
DEL
```

## Write flow
For `SET`:
```text
make WAL Set record
append record to WAL file
apply SET to in-memory storage
```

For `DEL`:
```text
make WAL Del record
append record to WAL file
apply DEL to in-memory storage
```

## Recovery flow
Recovery reads WAL records from file and replays them into in-memory storage.

Example:
```text
AKVWAL1 1 set 757365726e616d65 616c6578
AKVWAL1 2 set 726f6c65 61646d696e
AKVWAL1 3 del 757365726e616d65 -
```

Recovered state:
```text
role = admin
username = missing
```

## Sequence numbers
The WAL-backed storage layer allocates monotonically increasing sequence numbers.

After recovery, the next sequence number is computed from the highest replayed sequence
number.

## Important durability note
This is still a foundation step.

It does not yet provide production durability guarantees.

Missing pieces include:
- fsync policy;
- checksum validation;
- corruption recovery;
- snapshots;
- compaction;
- `asterd` integration.
