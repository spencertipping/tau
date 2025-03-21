# UTF9
Values within [ζ](zeta.md) buffers are stored using the UTF9 protocol, which is inspired by [msgpack](https://msgpack.org) but optimized for in-place efficiency. UTF9 has a few invariants:

1. Values are immutable once serialized
2. Serialization claims a length before writing data
3. Most of UTF9 is fully portable and always big-endian, although there are a few non-portable values, e.g. for oversized heap allocations that don't fit inline into ζ (or anything to do with π)
4. Nested values are self-contained and can be accessed just by address
5. ∃ toplevel stream markers that inform the state of computation without modifying it, and these stream markers may be embedded into other data structures freely
6. For large data structures, indexes can be generated alongside the value to provide fast lookup


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
flag        = 1 bit
size_marker = 2 bits
```

For example, the binary byte `01101010` (`0x6a`) has type marker `01101` (`0xb`), flag `0`, and size marker `10` (`0x02`).


### Flags
Flags have undefined semantics at the UTF9 protocol level. They union upwards; that is, any value that contains a flagged value will itself be flagged. In practice, π uses flags to optimize GC; see [π GC](pi-gc.md) and [π UTF9](pi-utf9.md) for details.


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
| `0 1010` | complex f32           |
| `0 1011` | complex f64           |
| `0 1100` | boolean/bitvector     |
| `0 1101` | symbol (empty = null) |
| `0 1110` | stream manipulator    |
| `0 1111` | **reserved**          |
| `1 0000` | bytes                 |
| `1 0001` | utf8                  |
| `1 0010` | index                 |
| `1 0011` | **reserved**          |
| `1 0100` | tuple                 |
| `1 0101` | map                   |
| `1 0110` | set                   |
| `1 0111` | tensor                |
| `1 1000` | π-scoped object       |
| `1 1001` | Φ-scoped object       |
| `1 1010` | host-scoped object    |
| `1 1011` | build-scoped object   |
| `1 1100` | **reserved**          |
| `1 1101` | **reserved**          |
| `1 1110` | `none`                |
| `1 1111` | `frame`               |


### Size codes
Size bytes, when indicated, immediately follow the control byte -- that is, they occur before any bytes related to the type.

| Bits | Following bytes | Size      |
|------|-----------------|-----------|
| `00` | `u8 n`          | `n` bytes |
| `01` | `u16 n`         | `n` bytes |
| `10` | `u32 n`         | `n` bytes |
| `11` | `u64 n`         | `n` bytes |


## Type-specific formats
Most types are self-explanatory: the value, in big-endian where applicable, immediately follows the control byte and any size bytes.

**NOTE:** the encoded size describes the data itself and doesn't include the control+size bytes. So `00011000 08 11 22 33 44 55 66 77 88` is a valid `u64` whose encoded size is 8, but which actually occupies 10 bytes.


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

For example, `0x03 0x08` (8-byte `i8`) followed by `1122334455667788` will be interpreted as `i8[8]`, and any math will be broadcast accordingly. Implicit vectors have no specified direction; they can be used as row or column vectors in any matrix context.

In contrast, `bytes` and `utf8` are always single, variable-size elements; the control-byte size determines how many bytes each contains.

**NOTE:** it is illegal to specify a length that is not an even multiple of the thing being vectorized. The value will not corrupt the rest of the datastream, but will be interpreted in an undefined way if you do this.


### Booleans
`0` for false, `1` for true. Booleans can be vectorized into bitsets; in that case, `b[i] = xs[i >> 3] & (1 << i & 7)`; that is, the least-significant bit of the first byte has index `0`.

The least-significant four bits of the first byte are used to encode the size-remainder, in this case `n & 15`. For example, suppose we have this:

```
       n % 16 = 12 (n == 12)
       |
       | bits 4..11
       V VV
60 02 fc ff
^   ^ ^
|   | bits 0..3
|   |
|   2bytes
bool
```

We use the half-byte strategy like this so single boolean values use only three bytes. Technically we could encode the remainder with just three bits, but I prefer to use four to keep the encoding more predictable.

Calculating the logical size of a bitvector from its byte-size + remainder requires us to break on the remainder. If _n%8 ≤ 4_, then _n = 8(s - 1) + n%8_; otherwise _n = 8(s - 2) + n%8_.

| n  | n%8 | size |
|----|-----|------|
| 0  | 0   | 1    |
| 1  | 1   | 1    |
| 2  | 2   | 1    |
| 3  | 3   | 1    |
| 4  | 4   | 1    |
| 5  | 5   | 2    |
| 6  | 6   | 2    |
| 7  | 7   | 2    |
| 8  | 0   | 2    |
| 9  | 1   | 2    |
| 10 | 2   | 2    |
| 11 | 3   | 2    |
| 12 | 4   | 2    |
| 13 | 5   | 3    |
| 14 | 6   | 3    |
| 15 | 7   | 3    |
| 16 | 0   | 3    |


### Tensors
_n_-dimensional packed arrays of some numeric type. Tensors are formatted as two concatenated vectors, the first of which is the dimensional encoding and the second is the data:

```
cb [sb] dim_cb  [dim_sb]  d1 d2 ... dk
        data_cb [data_sb] x1 x2 ... xn
```

`dim` is an unsigned integral type, e.g. `u32`, whose size covers one or more such numbers that define the tensor's size along each dimension.

`data` is a numeric type, e.g. `cf64`, whose size covers the tensor data itself. The first dimension varies last, so `dim` is a radix encoding vector.

Just as with vectors, the data size and dimensions must line up for the tensor to be valid: `|data| = ∏dim`.


### Signed vs unsigned ints
Signed ints are promoted to `i64` before doing arithmetic, unsigned ints are allowed to overflow within their declared size. Signed int overflow is undefined, just like in C/C++.

As a rule, use unsigned if you want specific overflow semantics and signed if you want "an int that's big enough" and don't want to set an upper bound on its size.


### Symbols
Symbols are just integers, but they exist within a separate namespace to prevent collisions. Their mapping to strings (or anything else) is not specified. They can be any length, e.g. `01101000 00100000 ...` would encode a 32-byte symbol that could hold a SHA256. This could also be represented with a vectorized int of any size, but symbol equivalence will be a single op, as opposed to the vectorized `==` for ints.

Semantically, a symbol of zero length is defined to mean `null` or `nil`.


### Stream state manipulators
```
cb [sb] 01 dim            ← α
cb [sb] 02 dim            ← ω
cb [sb] 03 dim            ← τ
cb [sb] 04 dim [theta...] ← θ
cb [sb] 05 dim [iota...]  ← ι
cb [sb] 06 dim            ← κ
```

`dim` is a single byte describing which dimension is being described. _τ²_, for example, describes a sub-cycle within the larger _τ_-cycle -- sort of like a cross-toroidal orbit. `dim` begins at 1. Zero is currently reserved.

_α_ indicates beginning-of-stream, is usually optional, and often is not provided.

Here, `theta...` is a big-endian unsigned integer (1, 2, 4, or 8 bytes) that represents the approximate stream angle -- that is, the estimated fraction of items seen so far until the next _τ_.

_ω_ is equivalent to closing the stream: sending it will cause the stream to be closed from the sender's side. Receivers will never receive this value directly.

_ι_ and _κ_ are used by some operators for flow control. They are sent _backwards_ up a pipe as an alternative to using buffer-fill as the traffic control mechanism. Not all operators are required to respect these symbols.


### Indexes
Indexes provide `hash → offset` lookups (or, for tuples or `utf8`, `index → offset`) for the primary access method for a container or UTF8 string.

Indexes are formatted like this:

```
cb [sb] (index-vector) (container) (bits)
        |-------------------------------| <- data size
                       |
                       |   <- offsets relative to beginning of container
```

`bits` is metadata that specifies the number of bits used to skip through the data to construct the index table. This is included so the [π GC](pi-gc.md) can preserve index resolution. It's just a `u8`.

**NOTE:** the index is placed before the container (1) for cache locality; and (2) because it's typically written as the container itself is being written, to avoid buffering.

The index vector is a vectorized unsigned type that contains `k₁ o₁ k₂ o₂ ...`. These can be encoded using any unsigned width large enough to refer to the container byte offsets.

The container's type determines what `k` means. For tuples, `k` is a zero-based integer subscript; for sets or maps, `k` is the highest bits of the hash of the element in question (for maps, the key; for sets, the whole element). This means index keys will always be sorted and, with the possible exception of heterogeneous tuples, uniformly distributed; as such, the keyspace can be interpolation-searched.

UTF8 strings can be indexed; in that case, `(container)` is a `utf8` and the index keys are decoded character positions within the string.


### Φ-scoped object
A UTF9 toplevel for values whose meaning becomes undefined outside of the Φ in which they were created. This typically refers to unqualified FDs and un-shared memory addresses.

**NOTE:** Φ, host, and build-scoped objects can all be sent into locations where they are invalid; they just can't be _used_ there. It's legal to send one of these objects to an invalid location only to later send it back to where it's defined and use it there. Each scoped object contains a marker that indicates whether it can be used from a given Φ.

See [UTF9 Φ scope](utf9-Phi-scope.md) for more details.


### Host-scoped object
A UTF9 toplevel for values that become undefined outside of the host on which they were created. Here, "host" means "abstracted kernel" -- this includes things like a container or VM. You can assume that a shared PID/IPC space is sufficient for host continuity.

See [UTF9 host scope](utf9-host-scope.md) for more details.


### Build-scoped object
A UTF9 toplevel for values that incorporate platform/architecture specifics; these are valid within any copy of the same native build. If we were to generate executable code at runtime and ship it in UTF9, that would be build-scoped.

See [UTF9 build-scope](utf9-build-scope.md) for more details.


### π internals
These are values reserved for [π](pi.md), in particular its memory allocation mechanics. They are assumed to be opaque to everyone else, and are portable only within a single π instance. See [π GC](pi-gc.md) for details.

**NOTE:** π assigns GC semantics to the flag bit. Within the π context, flags cannot be used for any other purpose -- which means π exclusively owns the flag bit when in use.


### `none`
`none` represents a value that does not exist or could not be computed -- like an error. In general, `none` propagates through any function that inspects a value beyond its type. See the full [UTF9 `none` spec](utf9-none.md) for more details.


### Frames
If you're storing a UTF9 datastream someplace where its type is not known by default, it should begin with a frame. Because the frame byte always takes the form `11111___`, it is illegal in UTF-8 -- which quickly marks the data as binary.

From UTF9's internal perspective, a frame looks like a tuple with some metadata; that is, it can contain zero or more UTF9 values.

**TODO:** frame spec
