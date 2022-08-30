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
| `1001_`     | indexes                                     |
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
| `1 0010` | single index              |
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


## Type-specific formats
Most types are self-explanatory: the value, in big-endian where applicable, immediately follows the control byte and any size bytes.


### Collections
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

Unindexed collections have no constraints on element ordering.

Indexed `map` and `set` are sorted by `hash(k)` and `hash(x)`, respectively. `tuple` has no ordering, which means unassisted lookups are _O(n)_. Indexed lookups are much faster; see [indexes](#indexes).


### Implicit vectorization
Implicit vectorization happens with fixed-size primitives: `iN`, `uN`, `fN`, `cN`, and, in the future, `b`.

For example, `0x03` (8-byte `i8`) followed by `1122334455667788` will be interpreted as `i8[8]`, and any math will be broadcast accordingly. Implicit vectors have no specified direction; they can be used as row or column vectors in any matrix context.

In contrast, `bytes` and `utf8` are always single, variable-size elements; the control-byte size determines how many bytes each contains.

**NOTE:** it is illegal to specify a length that is not an even multiple of the thing being vectorized. The value will not corrupt the rest of the datastream, but will be interpreted in an undefined way if you do this.


### Booleans
`0` for false, `1` for true. Booleans can be vectorized into bitsets; in that case, `b[i] = xs[i >> 3] >> (i & 7) & 1`; that is, the least-significant bit of the first byte has index `0`.


### Tensors
_n_-dimensional packed arrays of some numeric type. Tensors are formatted like this:

```
cb [sb] dim_cb  [dim_sb]  d1 d2 ... dk
        data_cb [data_sb] x1 x2 ... xn
```

`dim` is an unsigned integral type, e.g. `u32`, whose size covers one or more such numbers that define the tensor's size along each dimension.

`data` is a numeric type, e.g. `cf64`, whose size covers the tensor data itself. The first dimension varies last, so `dim` is a radix encoding vector.


### Signed vs unsigned ints
Signed ints are promoted to `i64` before doing arithmetic, unsigned ints are allowed to overflow within their declared size. Signed int overflow is undefined, just like in C/C++.

As a rule, use unsigned if you want specific overflow semantics and signed if you want "an int that's big enough" and don't want to set an upper bound on its size.


### Symbols
Symbols are just integers, but they exist within a separate namespace to prevent collisions. Their mapping to strings (or anything else) is not specified. They can be any length, e.g. `01101100 00100000 ...` would encode a 32-byte symbol that could hold a SHA256. This could also be represented with a vectorized int of any size, but symbol equivalence will be a single op, as opposed to the vectorized `==` for ints.

Semantically, the `0` symbol always means `null` or `nil`.


### Stream state manipulators
```
cb [sb] 01            ← ω
cb [sb] 02            ← τ
cb [sb] 03 [theta...] ← θ
cb [sb] 04 [iota...]  ← ι
cb [sb] 05            ← κ
```

Here, `theta...` is a big-endian unsigned integer (1, 2, 4, or 8 bytes) that represents the approximate stream angle -- that is, the estimated fraction of items seen so far until the next _τ_.

_ω_ is equivalent to closing the stream: sending it will cause the stream to be closed from the sender's side. Receivers will never receive this value directly.

_ι_ and _κ_ are used by some operators for flow control. They are sent _backwards_ up a pipe as an alternative to using buffer-fill as the traffic control mechanism. Not all operators are required to respect these symbols.


### Indexes
Indexes provide `hash → offset` lookups (or, for tuples or `utf8`, `index → offset`) for the primary access method for a container or UTF8 string.

Indexes are formatted like this:

```
cb [sb] (container) k1 o1 k2 o2 ... kn on
        |-------------------------------|  <- data size
        |
        |   <- offsets relative to beginning of container
```

**The container's size encoding matters,** as it specifies the width of `ki` and `oi`. For example, if the container's size is specified as `_____101 xx yy` (`u16` size), then each index entry will have a `u16 k` and `u16 o`.

The container's type determines what `k` means. For tuples, `k` is a zero-based integer subscript; for sets or maps, `k` is the highest bits of the hash of the element in question (for maps, the key; for sets, the whole element). This means index keys will always be sorted and, with the possible exception of heterogeneous tuples, uniformly distributed; as such, the keyspace can be interpolation-searched.

UTF8 strings can be indexed; in that case, `(container)` is a `utf8` and the index keys are decoded character positions within the string.


### Non-portable `(pid, fd)`
It's sometimes useful to capture an FD from one process and send it to another; to do this, we can store a value inside a `pidfd` and serialize it across IPC lines, as long as the value never leaves the machine/container it's running on.

Note that τ doesn't promise to stop you from sending pid/fds elsewhere, they just won't work outside of their original context.

Also note that pid/fds may create race conditions, since the other process can close a file before you have accessed it.


### Non-portable heap refs
Some UTF9 values will be too large for the ζ being used to carry them. Rather than expanding ζ or failing, we can "box" the value into a specially-allocated `u8[]`, then store a reference to that `u8[]` into ζ.

**NOTE:** non-portable refs cannot be embedded into other data structures, even for local transit. τ data structures don't internally hold references to anything or support tracing GC, so it would defeat automatic memory allocation if these structures could be serialized into anything besides the toplevel.
