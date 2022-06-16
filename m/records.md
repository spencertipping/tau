# UTF9 records
Key characteristics of records:

+ Efficient to serialize/deserialize, both in space and time
+ Able to hold large (>1TiB) amounts of data
+ Efficient to left-cons and left-match fields, e.g. for multiplexing
+ Easy to encode fields of commonly-used datatypes
+ Fast C++ API for tau record fields
+ Side-channel symbols: _α_, _ω_, _ι_, _κ_, and _τ(x)_

There are several big parts of the `utf9` bytecode:

1. The bytecode itself, used to store values
2. The typecode, a truncated subset used to encode array element types
3. Domain projection, the function applied to create orderings and hashes
4. Transit containers, used to guarantee message atomicity
5. Disk containers, used to guarantee atomicity + integrity
6. Greek symbols, which are side-band data


## Greek symbols
+ _α_ is a "time begins now" message sent by newly-connected stream elements
+ _ω_ is an error/EOF marker that means "time has ended and will never resume"
+ _ι_ means "you can start sending data now"
+ _κ_ means "you cannot send more data now; it will block"

_τ_ is different in that it has two forms. _τ₀_ means "you are now starting a new time-cycle" -- which resets any ordering active on a stream, like a mini-EOF. _τ(x)_ means "you are at roughly this point in the time-cycle" and is used to provide estimates of the total stream size. _τ(x)_ need not be monotonic; it's just an estimate.


## Bytecode
`utf9` is inspired by `msgpack` and designed to be usable in-place, that is without "unpacking" into an in-memory datastructure. This means a few things:

1. Containers within `utf9` records are lazily loaded
2. Containers prepend byte-lengths, indexes, and complexity measures
3. Functionally speaking, `utf9` behaves like a micro-heap that is GC'd prior to serialization

Lazy loading means that a `utf9` record view must store a diff in order to provide mutability. This diff can have a ravel, which means we can stream values into a `utf9` diff reduction to construct a final object (emitted on _τ_). This model unifies diff-streaming and atomic record edits, providing a nice scaffold for later features like OT -- nothing stops us from defining an OT diff reducer that reorders edits on user-submitted time and emits the reconciled state.

In other words, [row transformation](transform.md) amounts to generating modification ops into a suitable reduction context. That makes row transforms differentiable by time. It also means we can reuse all of the row-transformation operations as streaming things, with _τ_ markers to commit/emit the current state.

**NOTE:** this brings delta operators like `(+ 5)` and map-insert-and-merge front and center. We should have a broad dictionary of these things to use within row context.

**NOTE:** `utf9` records may be aliased in memory, in which case they shouldn't be modified; then all updates will be applied as diff patches.


### Design
Bytecode-encoded objects need not point to the origin; that is, there are no parent links (just like `msgpack`). Each object must be decodable with no base-pointer context, meaning we don't keep a base pointer and there is no value aliasing.

Modifications are also bytecodes of a sort, but indirectly encoded using symbols.

Like `msgpack`, we optimize for brevity by providing `fix*` variants for small structures.


### Unused (reserved) bytes
+ `0x0c-0x0f`
+ `0x50-0x5f`
+ `0x60-0x6f`
+ `0x70-0x7f`


### Atomic types
| Byte        | Following bytes | Description                  |
|-------------|-----------------|------------------------------|
| `0x00`      | 1               | `uint8`                      |
| `0x01`      | 2               | `uint16`                     |
| `0x02`      | 4               | `uint32`                     |
| `0x03`      | 8               | `uint64`                     |
| `0x04`      | 1               | `int8`                       |
| `0x05`      | 2               | `int16`                      |
| `0x06`      | 4               | `int32`                      |
| `0x07`      | 8               | `int64`                      |
| `0x08`      | 4               | `float32`                    |
| `0x09`      | 8               | `float64`                    |
| `0x0a`      | 8               | `symbol`                     |
| `0x0b`      | 8               | `pidfd`, `uint32` pid and fd |
| `0x10`      | 0               | `alpha`                      |
| `0x11`      | 0               | `omega`                      |
| `0x12`      | 0               | `iota`                       |
| `0x13`      | 0               | `kappa`                      |
| `0x14`      | 0               | `tau0`                       |
| `0x15`      | 2               | `tau`, `uint16 x`            |
| `0x16`      | 4               | `tau`, `uint32 x`            |
| `0x17`      | 8               | `tau`, `uint64 x`            |
| `0x18`      | 1 + len         | `utf8`, 8-bit _byte_ length  |
| `0x19`      | 2 + len         | `utf8`, 16-bit _byte_ length |
| `0x1a`      | 4 + len         | `utf8`, 32-bit _byte_ length |
| `0x1b`      | 8 + len         | `utf8`, 64-bit _byte_ length |
| `0x1c`      | 1 + len         | `bytes`, 8-bit length        |
| `0x1d`      | 2 + len         | `bytes`, 16-bit length       |
| `0x1e`      | 4 + len         | `bytes`, 32-bit length       |
| `0x1f`      | 8 + len         | `bytes`, 64-bit length       |
| `0x2N`      | `N`             | `fixutf8`                    |
| `0x3N`      | `N`             | `fixbytes`                   |
| `0x80-0xff` | 0               | `fixint`                     |

**NOTE:** byte ordering is always big-endian.

Note that both `utf8` and `bytes` encode the _byte_ length of the payload, not the logical number of elements as you might expect. This makes it possible to skip over the value without running a UTF-8 decode loop.


### Simple containers
| Byte        | Following bytes | Description                                   |
|-------------|-----------------|-----------------------------------------------|
| `0x40`      | `l n xs...`     | `tuple` of length `int8 l`,  `int8` elements  |
| `0x41`      | `l n xs...`     | `tuple` of length `int16 l`, `int16` elements |
| `0x42`      | `l n xs...`     | `tuple` of length `int32 l`, `int32` elements |
| `0x43`      | `l n xs...`     | `tuple` of length `int64 l`, `int64` elements |
| `0x44`      | `l n t xs...`   | `array<t>` of length `int8 l`,  `int8 n`      |
| `0x45`      | `l n t xs...`   | `array<t>` of length `int16 l`, `int16 n`     |
| `0x46`      | `l n t xs...`   | `array<t>` of length `int32 l`, `int32 n`     |
| `0x47`      | `l n t xs...`   | `array<t>` of length `int64 l`, `int64 n`     |
| `0x48-0x4f` | `l xs...`       | `fixtuple`, byte-length `int8 l`              |

**NOTE:** array `l = len(t) + len(xs)`, not just `len(xs)` as it is for tuples.


#### Element typecodes
Arrays distribute a type prefix across a series of elements; for example, `array int8 5` would then be followed by five bytes, each of which would be interpreted as though it had been specified with `int8`. The type prefix `t` need not be a single byte; you can have `array utf8 5 5` for an array of five-byte UTF-8 strings. You can also have arrays within arrays.

Array typecodes are identical to regular bytecodes, but with two major changes:

1. Single-byte bytecodes aren't supported
2. The value isn't encoded (the array effectively distributes it)

Examples of array element types:

+ `tuple 48`
+ `array 5 4 int8` -- note `5 = len(int8 bytecode) + 4`
+ `fixtuple4 48`
+ `utf8 10`

When `array` is used as an array element, its `l` should be the length of each packed thing; that is, `array l n t` would set `l = n * len(x) = len(xs)`. `t` is no longer added to the length because it's packed out.


### Container indexes
**TODO:** rework this format to eliminate some duplication and increase cache locality

Container indexes modify simple containers by prepending an index that provides fast lookups. The container's values are usually sorted by the indexed field for content-indexed lookups. Large heterogeneous tuples can be indexed by element subscripts. All indexes map some aspect of the data to the byte offset of the container element that contains it.

Indexes can be complete (`cidx`) or incomplete (`iidx`). Complete indexes provide a mapping for every element, incomplete indexes don't. Incomplete indexes enable fast interpolation search, complete indexes allow elements to be found even when they aren't in sorted order. Unordered elements must be addressed with a complete index; otherwise an index miss would result in a full linear scan.

Indexes are tuples of `(ks, ka, ia)`. `ks` is a keyspec, which specifies which value is being indexed. `ka` and `ia` are the array of keys and the array of seek-indexes, respectively.

| Byte | Following bytes | Description                       |
|------|-----------------|-----------------------------------|
|      | `l ks ka ia`    | `cidx` with byte-length `int16 l` |
|      | `l ks ka ia`    | `cidx` with byte-length `int32 l` |
|      | `l ks ka ia`    | `cidx` with byte-length `int64 l` |
|      | `l ks ka ia`    | `iidx` with byte-length `int8 l`  |
|      | `l ks ka ia`    | `iidx` with byte-length `int16 l` |
|      | `l ks ka ia`    | `iidx` with byte-length `int32 l` |
|      | `l ks ka ia`    | `iidx` with byte-length `int64 l` |

The data structure being indexed occurs immediately after the index. Logically, it's treated as a part of the index itself (which yields "a map" or "a set") but the two are decoded independently.

**NOTE:** multiple indexes can be stacked. For example, we can have an associative map of `(k v)` tuples that is indexed both by `i` numerical indexes and `k` ordering.

**TODO:** eliminate duplicate lengths for `ka` and `ia` by having indexes be a custom array type


#### Keyspec
The keyspec is a symbol that describes the aspect of the data that's being indexed. It is currently one of:

+ For whole elements, e.g. sets
  + `i`: the element position
  + `h`: `hash(element)`, possibly truncated to just the top _n_ bits
+ For `(k v₁ v₂ ...)` associative tuples
  + `k`: the initial element value
  + `kh`: `hash(element[0])`, possibly truncated to the just the top _n_ bits

**TODO:** add further index capability, e.g. `x[i]` for custom `i`

**TODO:** make these `fixint`s, not symbols

The index type influences the idiomatic data structure preference:

+ `i`: array
+ `h`: set
+ `k` and `kh`: associative map/multimap


#### Intrinsic hashing and ordering
All `utf9` datatypes have default intrinsic hashes and ordering, both of which are guaranteed to be stable across platforms and architectures. This makes it possible to persist ordered/hashed things on one system and use them elsewhere.

Ordering is defined by projecting each type into a domain and comparing within that domain.


#### Seek arrays
`ka` is an array of fixed-size keys and `ia` is an array of fixed-size seek offsets (relative to the container base). `ka` is always ordered, but _`ia` need not be forward-ordered for complete indexes._ This allows container tuples to act like databases, providing multiple parallel indexes for the same data.

`ka` can have any type as long as an ordering exists. `ia` is always `int16`, `int32`, or `int64`.


## Transit spec
A lightweight framing protocol that wraps one or more records over non-persistent connections, e.g. pipes or networks. There is no integrity verification or alignment.

```cpp
struct utf9_transit_frame
{
  uint8_t const magic[4] = {0xff, 'u', '9', 0x00};
  uint64_t      length;
  uint8_t       frame[length];
};
```

Records are framed for transit so we can preload the length and create a buffer to read them; otherwise we may read beyond end-of-buffer when determining array sizes.

**TODO:** is it worth having array sizes be directly encoded to avoid the array-of-arrays problem? Probably not; we won't spend much effort decoding those, and we have dedicated indexes to avoid excessive jumping.


## Disk spec
Similar to the transit spec, but intended for long-term persistence and easy lookup. In particular, we want a few features to work well from an IO perspective:

1. File splitting: seek to somewhere in the middle and find record boundaries
2. Optional robustness checking, both per-record and per-file
3. Fast record skipping for the sequential-access case
4. Preallocated space
5. File-level indexes


### Persistent header
Disk-persisted `utf9` frames begin with a header that establishes some context, including the frame boundary.

```cpp
#define VERSION 0x01

struct utf9_disk_header
{
  uint8_t const magic[8] = {0xff, 0xff, 'u', 't', 'f', '9', 0x00, VERSION};
  int128        secret;
    // int128   boundary = sha256(secret) >> 128  <- readers calculate this
};
```


### `utf9` frames
Frames are 16-byte aligned and begin with the boundary value, the frame key, and the signed frame key.

```cpp
struct utf9_disk_frame_header
{
  int128 const  boundary   = utf9_disk_header.boundary;
  int128        frame_sha  = sha256(frame) >> 128;
  int128        signed_sha = sha256(frame_sha ^ utf9_disk_header.secret) >> 128;
  uint8_t const magic[8]   = {0xff, 0xff, 'u', 't', 'f', '9', 'f', 0x00};
  uint64_t      length;  // NOTE: always big-endian
};

struct utf9_disk_frame
{
  utf9_disk_frame_header header;
  uint8_t                frame  [header.length];
  uint8_t const          padding[header.length + 15 & ~15] = {0};
};
```

`sizeof(utf9_frame_header) == 64`. Because frames carry the header overhead, each frame can contain multiple records emitted back to back. The frame is there to provide integrity checking for its contents and to provide a seek point within the file, making it possible to jump to an arbitrary location and find a nearby frame boundary.

`boundary` decreases the amount of overhead required to scan for a frame boundary, while `record_key` and `signed_key` provide robust confirmation.

**TODO:** add the chain SHA
