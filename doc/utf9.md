# UTF9
Values within [ζ](zeta.md) buffers are stored using the UTF9 protocol, which is inspired by [msgpack](https://msgpack.org) but optimized for in-place efficiency. UTF9 has a few invariants:

1. Values are immutable once serialized
2. Serialization claims a length before writing data
3. Most of UTF9 is fully portable and fixed-endian, although there are a few non-portable values, e.g. for oversized heap allocations that don't fit inline into ζ
4. Nested values are self-contained and can be accessed just by address
5. ∃ toplevel stream markers that inform the state of computation without modifying it, and these stream markers may be embedded into other data structures freely
6. For nested or large data structures, indexes are generated alongside the value to provide fast lookup


## Stream states
UTF9 provides the following side-channel markers:

+ _ω:_ end of data; no further values can be read from ζ
+ _τ:_ end of cycle; any data orderings are now reset, but more data may exist
+ _θ(x):_ approximately _x·τ_ radians into the current cycle

These are all first-class data, enabling them to be bundled and sent over a multiplexed connection.


## Control byte
UTF9 is meant to be an interchange format much like JSON or msgpack, which means we need to encode both the size and semantic type of each value. Both are done in a single header byte, split into two pieces:

```
type_marker = 5 bits
size_marker = 3 bits
```

For example, the binary byte `01101010` (`0x6a`) has type marker `01101` (`0xb`) and size marker `010` (`0x02`).


### Typecodes
| Bits     | Type                  |
|----------|-----------------------|
| `0 0000` | i8                    |
| `0 0001` | i16                   |
| `0 0010` | i32                   |
| `0 0011` | i64                   |
| `0 0100` | u8                    |
| `0 0101` | u16                   |
| `0 0110` | u32                   |
| `0 0111` | u64                   |
| `0 1000` | f32                   |
| `0 1001` | f64                   |
| `0 1010` | bytes                 |
| `0 1011` | utf8                  |
| `0 1100` | stream state          |
| `0 1101` | reserved              |
| `0 1110` | reserved              |
| `0 1111` | reserved              |
| `1 0000` | tuple                 |
| `1 0001` | map                   |
| `1 0010` | set                   |
| `1 1111` | native (non-portable) |


### Size codes
Size bytes, when indicated, immediately follow the control byte -- that is, they occur before any bytes related to the type.

| Bits  | Following bytes | Size      |
|-------|-----------------|-----------|
| `000` |                 | 1 byte    |
| `001` |                 | 2 bytes   |
| `010` |                 | 4 bytes   |
| `011` |                 | 8 bytes   |
| `100` | `u8 n`          | `n` bytes |
| `101` | `u16 n`         | `n` bytes |
| `110` | `u32 n`         | `n` bytes |
| `111` | `u64 n`         | `n` bytes |


### Vectorization
`i8` is one byte by definition, so what happens if we use a larger size with it? For example, `03` (8-byte `i8`) followed by `1122334455667788`? This will be interpreted as `i8[8]`, and any math will be broadcast accordingly.
