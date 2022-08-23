# UTF9
Values within [ζ](zeta.md) buffers are stored using the UTF9 protocol, which is inspired by [msgpack](https://msgpack.org) but optimized for in-place efficiency. UTF9 has a few invariants:

1. Values are immutable once serialized
2. Serialization claims a length before writing data
3. Most of UTF9 is fully portable and always big-endian, although there are a few non-portable values, e.g. for oversized heap allocations that don't fit inline into ζ
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
Broadly:

| Bit pattern | Types                                       |
|-------------|---------------------------------------------|
| `00___`     | fixed-size, vectorizable ints               |
| `010__`     | fixed-size, vectorizable floats and complex |
| `011__`     | categorical data types                      |
| `1000_`     | string-like data types                      |
| `1001_`     | composite index                             |
| `101__`     | collections (tuples, maps, sets, tensors)   |
| `110__`     | non-portable values (native-endian)         |
| `111__`     | reserved                                    |

In detail:

| Bits     | Type                      |
|----------|---------------------------|
| `0 0000` | i8                        |
| `0 0001` | i16                       |
| `0 0010` | i32                       |
| `0 0011` | i64                       |
| `0 0100` | u8                        |
| `0 0101` | u16                       |
| `0 0110` | u32                       |
| `0 0111` | u64                       |
| `0 1000` | f32                       |
| `0 1001` | f64                       |
| `0 1010` | complex f32               |
| `0 1011` | complex f64               |
| `0 1100` | boolean                   |
| `0 1101` | opaque symbol (0 = null)  |
| `0 1110` | stream manipulator        |
| `0 1111` | **reserved**              |
| `1 0000` | bytes                     |
| `1 0001` | utf8                      |
| `1 0010` | composite index           |
| `1 0011` | **reserved**              |
| `1 0100` | tuple                     |
| `1 0101` | map                       |
| `1 0110` | set                       |
| `1 0111` | tensor                    |
| `1 1000` | `pid, fd` (non-portable)  |
| `1 1001` | `heap ref` (non-portable) |
| `1 1010` | **reserved**              |
| `1 1011` | **reserved**              |
| `1 1100` | **reserved**              |
| `1 1101` | **reserved**              |
| `1 1110` | **reserved**              |
| `1 1111` | **reserved**              |


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


### Implicit vectorization
Implicit vectorization happens with any type that has an opinion about its size.

For example, `0x03` (8-byte `i8`) followed by `1122334455667788` will be interpreted as `i8[8]`, and any math will be broadcast accordingly. Implicit vectors have no specified direction; they can be used as row or column vectors in any matrix context.

In contrast, `bytes` and `utf8` are always single, variable-size elements; the control-byte size determines how many bytes each contains.


## Type-specific formats
Most types are self-explanatory: the value, in big-endian where applicable, immediately follows the control byte and any size bytes.

Collections don't contain indexing data inline. Here's what each one looks like:

```
        control byte
        |
        |   size bytes
        |   |
tuple: cb [sb] t[0] t[1] ... t[n]
               |----------------|  <- data size

map:   cb [sb] k0 v0 k1 v1 ... kn vn
               |-------------------|

set:   cb [sb] x1 x2 ... xn
               |----------|
```

`map` and `set` are sorted by `hash(k)` and `hash(x)`, respectively. `tuple` has no ordering, which means unassisted lookups are _O(n)_.

**TODO:** `tensor` spec


### Indexes
Indexes make it easy to seek to specific positions within a nested data structure.
