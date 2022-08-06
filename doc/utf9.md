# UTF9
Key characteristics of UTF9:

+ Efficient to serialize/deserialize, both in space and time
+ Able to hold large (>1TiB) amounts of data
+ Efficient to left-cons and left-match fields, e.g. for multiplexing
+ Easy to encode fields of commonly-used datatypes
+ Fast C++ API for tau record fields
+ Side-channel symbols: _α_, _ω_, _ι_, _κ_, _τ_, _θ_, and _ρ_

There are several big parts of the UTF9 bytecode:

1. The bytecode itself, used to store values
2. The typecode, a truncated subset used to encode array element types
3. Domain projection, the function applied to create orderings and hashes
4. Transit containers, used to guarantee message atomicity
5. Disk containers, used to guarantee atomicity + integrity
6. Greek symbols, which are side-band data


## Greek symbols
From the perspective of a single UTF9 datastream:

+ _α_ is a "time begins now" message sent by newly-connected stream elements
+ _ω_ is an error/EOF marker that means "time has ended and will never resume"
+ _ι_ means "you can start sending data now"
+ _κ_ means "you cannot send more data now; it will block"
+ _τ_ means "end of cycle", and is intended as a logical marker

The others, _θ_ and _ρ_, provide ways to estimate the position of the next _τ_ or _ω_. _θ_ represents the distance as an angle, in terms of the amount of data already seen; a _θ(x)_ marker means "you are currently at fraction _x_ of the full datastream in this cycle."

_ρ_ expresses distance in absolute terms: _ρ(x)_ means "there are _x_ remaining entries/bytes/whatever until the end of this cycle."


## Bytecode
UTF9 is inspired by `msgpack` and designed to be usable in-place, that is without "unpacking" into an in-memory datastructure. This means a few things:

1. Containers within UTF9 records are lazily loaded
2. Containers prepend byte-lengths, indexes, and complexity measures
3. Functionally speaking, UTF9 behaves like a micro-heap that is GC'd prior to serialization

Lazy loading means that a UTF9 record view must store a diff in order to provide mutability. This diff can have a ravel, which means we can stream values into a UTF9 diff reduction to construct a final object (emitted on _τ_). This model unifies diff-streaming and atomic record edits, providing a nice scaffold for later features like OT -- nothing stops us from defining an OT diff reducer that reorders edits on user-submitted time and emits the reconciled state.


### Design
Bytecode-encoded objects need not point to the origin; that is, there are no parent links (just like `msgpack`). Each object must be decodable with no base-pointer context, meaning we don't keep a base pointer and there is no value aliasing.

Like `msgpack`, we optimize for brevity by providing `fix*` variants for small structures.

Any datatype with a non-constant length has the length prepended to it so we can quickly skip over the value. The length tends to exclude any fixed-length parts of the bytecode to avoid integer overflow (which requires us to use a larger int size).


### Unused (reserved) bytes
+ `0x0f`
+ `0x17`
+ `0x5f`
+ `0x60`-`0x6f`
+ `0x70`-`0x7f`


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
| `0x0c`      | 0               | `false`                      |
| `0x0d`      | 0               | `true`                       |
| `0x0e`      | 0               | `null`                       |
| `0x0f`      |                 | **reserved**                 |
| `0x10`      | 0               | `alpha`                      |
| `0x11`      | 0               | `iota`                       |
| `0x12`      | 0               | `kappa`                      |
| `0x13`      | 4               | `rho`, `uint32 x`            |
| `0x14`      | 4               | `theta`, `uint32 x`          |
| `0x15`      | 0               | `tau`                        |
| `0x16`      | 0               | `omega`                      |
| `0x17`      | 0               | **reserved**                 |
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

+ `tuple 32 4 int64 int64 float64 float64` -- a 32-byte tuple with four elements
+ `array 8 4 int16` -- an `int16[4]`, which occupies 8 bytes
+ `fixtuple4 48 int64 int64 int32 utf8 28` -- a 4-tuple of `(int64, int64, int32, utf8[28])`, occupying 48 bytes
+ `utf8 10` -- a 10-byte UTF8 string

When `array` is used as an array element, its `l` should be the length of each packed thing; that is, `array l n t` would set `l = n * len(x) = len(xs)`. `t` is no longer added to the length because it's packed out.


#### Array elements and performance
**NOTE:** array elements are meant to be addressed in aggregate, not individually; as a result, individual addressing will _copy_ elements out of the array. If you want individual addressing for large elements, you should use a tuple instead.


### Container indexes
Maps and sets are indexed structures in that finding an element requires less than _O(n)_ time. UTF9 implements three types of indexes:

+ `pos`: element position → byte offset
+ `set`: element hash → byte offset
+ `map`: element first member hash → byte offset (elements are _n_-tuples, _n ≥ 1_)

**NOTE:** most UTF9 datatypes have default intrinsic hashes and ordering, both of which are guaranteed to be stable across platforms and architectures. This makes it possible to persist ordered/hashed things on one system and use them elsewhere.

_In every case, the collection elements are sorted by the indexing axis._ This is important because it allows us to define partial indexes; that is, indexes that don't cover every element. This makes every index an interpolation basis.

Because indexes imply a container type, we have two markers, `map` and `set`, that provide no index data but still indicate that the collection ahead is meant to be interpreted as a map or set, respectively. These are used for very small containers, e.g. small JSON maps, that wouldn't benefit from accelerated lookups but whose type information is still important.


#### Index bytecodes
| Byte   | Following bytes      | Description             |
|--------|----------------------|-------------------------|
| `0x50` | `l8 n8 bits vs...`   | pos8 index (list type)  |
| `0x51` | `l16 n16 bits vs...` | pos16 index (list type) |
| `0x52` | `l32 n32 bits vs...` | pos32 index (list type) |
| `0x53` | `l64 n64 bits vs...` | pos64 index (list type) |
| `0x54` | `l8 bits vs...`      | hashset8 index          |
| `0x55` | `l16 bits vs...`     | hashset16 index         |
| `0x56` | `l32 bits vs...`     | hashset32 index         |
| `0x57` | `l64 bits vs...`     | hashset64 index         |
| `0x58` | `l8 bits vs...`      | hashmap8 index          |
| `0x59` | `l16 bits vs...`     | hashmap16 index         |
| `0x5a` | `l32 bits vs...`     | hashmap32 index         |
| `0x5b` | `l64 bits vs...`     | hashmap64 index         |
| `0x5c` | 0                    | list type-hint          |
| `0x5d` | 0                    | set type-hint           |
| `0x5e` | 0                    | map type-hint           |

`lN` is defined as the length of the collection following the index; for example, the total size of a `hashset8` index would be `l8 + 3 + (~0ull >> bits)` -- that is, the collection length plus the bytecode, `l8` byte, and `bits` byte, plus the size of the `vs...` lookup table. The collection always immediately follows `vs...`.

**NOTE:** `0x5c`, `0x5d`, and `0x5e` merely hint the type of a tuple, so they don't require the tuple to be sorted in any particular way. These are otherwise equivalent to degenerate forms of `pos8`, `hashset8`, and `hashmap8`, but those require at least one extra byte each.

Position indexes translate `[i]` subscripts to byte-offsets within a tuple. They can be downsampled by a number of bits, encoded as an `int8`; for example, if `bits = 2`, then the byte-offset table encodes the positions of `[0]`, `[4]`, `[8]`, `[12]`, etc. This trades space for time, ultimately providing a random-accessibility continuum between tuples-as-linked-lists and tuples-as-arrays.


#### Index structure
Indexes are arrays of byte offsets into a tuple. For example, suppose we have a tuple of ints:

```
0         1        2       3       4       5         <- byte offsets
fixtuple4 length=4 fixint5 fixint5 fixint1 fixint64  <- bytecodes
```

Any index will be a packed array of offsets that refer to one of the tuple values directly, in this case `2`, `3`, `4`, or `5`. So we might have a positional index that looks like this:

```
0    1   2   3      4  5  6  7   8         9            <- byte offsets
pos8 l=6 n=4 bits=0 02 03 04 05  fixtuple4 length=4 ... <- bytecodes
                                 ^
                                 |
                                 tuple starts here
```


### Space/time tradeoffs
How do we set `bits`? The encoder needs two parameters:

1. The overhead ceiling per collection, as a fraction (e.g. the index should take up no more than 50% of the collection size)
2. The jump ceiling per seek, as an absolute number (e.g. no indexed retrieval should require more than _n_ cache misses)

(2) is optional; most of the time we use (1).


## Ordering
Some values have a total ordering defined for them. This is used in any context where values will be sorted, which includes locally for in-memory maps/sets and more broadly for streaming files of values.

**NOTE:** indexed collections cannot be compared, even for equality (τ draws no distinction between ordering and equality-checking). This means order-sensitive keys need to be simple.

The ordering function `O(x, y)` is defined for each type of value separately. Values of different types cannot be compared to one another.

```
O<uint>   (x, y) = x <=> y
O<int>    (x, y) = x <=> y
O<float32>(x, y) = x <=> y
O<float64>(x, y) = x <=> y
O<symbol> (x, y) = hash(x) <=> hash(y)
O<pidfd>  (x, y) = x.pid <=> y.pid || x.fd <=> y.fd
O<bool>   (x, y) = (false < true)

O<null> (x, y) = 0
O<greek>(x, y) = α < ι < κ < θ < ρ < τ < ω

O<utf8> (x, y) = memcmp(x[0..min], y[0..min]) || x.length <=> y.length
O<bytes>(x, y) = memcmp(x[0..min], y[0..min]) || x.length <=> y.length
O<tuple>(x, y) = x[0] <=> y[0] || O<tuple>(x[1..], y[1..]) || x.length <=> y.length
```


## Hashing
Some values can be hashed. Like ordering, hashing is stable across platforms, architectures, and degrees of semantic freedom within UTF9.

UTF9 hashing is defined in terms of [xxhash](https://cyan4973.github.io/xxHash/), from which we have two functions:

```cpp
uint64_t xxh(void*, size_t, uint64_t seed) = XXH64;
uint64_t xxc(uint64_t a, uint64_t b) { uint8_t bea[8] = big_endian(a); return XXH64(&bea, sizeof(bea), b); }
```

The hash function `H(x)` is defined like this:

```
H<uint>   (x) = xxc(x, 0x00)
H<int>    (x) = xxc(x, 0x04)
H<float32>(x) = xxh(big_endian(x), 4, 0x08)
H<float64>(x) = xxh(big_endian(x), 8, 0x09)
H<symbol> (x) = xxh(x.text, x.length, 0x0a)
H<pidfd>  (x) = xxc(xxc(x.pid, x.fd), 0x0b)
H<bool>   (x) = xxc(NULL, 0, x ? 0x0d : 0x0c)
H<null>   (x) = xxh(NULL, 0, 0x0e)

H<greek>  (x) = xxc(x, bytecode)

H<utf8>   (x) = xxh(x.data, x.length, 0x18)
H<bytes>  (x) = xxh(x.data, x.length, 0x1c)

H<tuple>  ((x, ...)) = xxc(H(x), H<tuple>((...)))
H<tuple>  (())       = xxh(NULL, 0, 0x20)
```


## Transit spec
A lightweight framing protocol that wraps one or more values over non-persistent connections, e.g. pipes or networks. There is no integrity verification or alignment because we assume all transit strategies are durable.

```cpp
struct utf9_transit_frame
{
  uint8_t const magic[4] = {0xff, 'u', '9', 0x00};
  uint64_t      length;
  uint8_t       frame[length];
};
```


## Disk spec
Similar to the transit spec, but intended for long-term persistence and easy lookup. In particular, we want a few features to work well from an IO perspective:

1. File splitting: seek to somewhere in the middle and find value boundaries
2. Optional robustness checking, both per-value and per-file
3. Fast value skipping for the sequential-access case
4. Preallocated space
5. File-level indexes


### Persistent header
Disk-persisted UTF9 frames begin with a header that establishes some context, including the frame boundary.

```cpp
#define VERSION 0x01

struct utf9_disk_header
{
  uint8_t const magic[8] = {0xff, 0xff, 'u', 't', 'f', '9', 0x00, VERSION};
  int128        secret;
    // int128   boundary = sha256(secret) >> 128  <- readers calculate this
};
```


### UTF9 frames
Frames are 16-byte aligned and begin with magic+length, the boundary value, the frame key, and the signed frame key.

```cpp
struct utf9_disk_frame_header
{
  uint8_t const magic[8]   = {0xff, 0xff, 'u', 't', 'f', '9', 'f', VERSION};
  uint64_t      length;  // NOTE: always big-endian
  int128 const  boundary   = utf9_disk_header.boundary;
  int128        frame_sha  = sha256(frame) >> 128;
  int128        signed_sha = sha256(frame_sha ++ utf9_disk_header.secret) >> 128;
  int128        chain_sha  = sha256(frame_sha ++ prev_frame.chain_sha) >> 128;
    // NOTE: prev_frame.chain_sha is utf9_disk_header.secret for the first frame
};

struct utf9_disk_frame
{
  utf9_disk_frame_header header;
  uint8_t                frame  [header.length];

  // FIXME: wrong bit-twiddly thing below I think
  uint8_t const          padding[header.length + 15 & ~15] = {0};
};
```

`sizeof(utf9_frame_header) == 80`. Because frames carry the header overhead, each frame can contain multiple values emitted back to back. The frame is there to provide integrity checking for its contents and to provide a seek point within the file, making it possible to jump to an arbitrary location and find a nearby frame boundary.

`boundary` decreases the amount of overhead required to scan for a frame boundary, while `frame_sha` and `signed_sha` provide robust confirmation. Padding means that `boundary` will always be 16-byte aligned.
