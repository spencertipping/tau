# Ωl
A header followed by many η values, each framed by its length. The header contains a `u64b` length field that describes the number of bytes in the file that are currently valid.

```
Ωl\0           # 4-byte magic
resv:u32b      # 4 bytes of reserved stuff, currently 0
size:u64b      # number of valid bytes (including header), updated after append
n₁:u32b η₁...  # n₁ bytes of η data, always at byte 16
n₂:u32b η₂...  # n₂ bytes of η data
...
```

Appenders acquire an exclusive lock against `size` for the duration of the append and `size` update. Otherwise no locking is used. Readers consult `size`, which will always be a valid endpoint because it is updated after the append that increases it.
