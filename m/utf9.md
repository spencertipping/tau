# UTF9
Key characteristics of `utf9`:

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

**NOTE:** `utf9` values may be aliased in memory, in which case they shouldn't be modified; then all updates will be applied as diff patches.


### Design
Bytecode-encoded objects need not point to the origin; that is, there are no parent links (just like `msgpack`). Each object must be decodable with no base-pointer context, meaning we don't keep a base pointer and there is no value aliasing.

Modifications are also bytecodes of a sort, but indirectly encoded using symbols.

Like `msgpack`, we optimize for brevity by providing `fix*` variants for small structures.

Any datatype with a non-constant length has the length prepended to it so we can quickly skip over the value. The length tends to exclude any fixed-length parts of the bytecode to avoid integer overflow (which requires us to use a larger int size).


### Unused (reserved) bytes
+ `0x0c-0x0f`
+ `0x60-0x63`
+ `0x70-0x73`


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
| `0x15`      | 8               | `tau`, `uint64 x`            |
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
+ `array 8 4 int16`
+ `fixtuple4 48 int64 int64 int32 utf8 28`
+ `utf8 10`

When `array` is used as an array element, its `l` should be the length of each packed thing; that is, `array l n t` would set `l = n * len(x) = len(xs)`. `t` is no longer added to the length because it's packed out.


#### Array elements and performance
**NOTE:** array elements are meant to be addressed in aggregate, not individually; as a result, individual addressing will _copy_ elements out of the array. If you want individual addressing for large elements, you should use a tuple instead.


### Container indexes
Indexes provide precise seek offsets for some or all elements in a collection, indexed by some function (often `x` for sets or `x[0]` for maps). The index covers every element if the collection is not already sorted along the index axis.

All `utf9` datatypes have default intrinsic hashes and ordering, both of which are guaranteed to be stable across platforms and architectures. This makes it possible to persist ordered/hashed things on one system and use them elsewhere.

**NOTE:** container indexes are intended for memory-resident structures, and are not optimized for disk-resident or framed data.


#### Index configurations
+ **key:** `pos` vs `set` vs `mapkey` vs `mapval`
+ **key order:** `hash-order` vs `compare-order`
+ **value order:** `ordered` vs `random`

Because multiple indexes can be prepended to the same container, it's possible to have a map+set+inverse map, all in one.

Value orderings impact the index in an important way: _a random index must include every element in the collection._ Value-ordered indexes need not include every element, since it's possible to interpolation-search and scan forwards.


#### Index bytecodes
| Byte   | Following bytes    | Description                 |
|--------|--------------------|-----------------------------|
| `0x50` | `l16 bits vs...`   | position index              |
| `0x51` | `l32 bits vs...`   | position index              |
| `0x52` | `l64 bits vs...`   | position index              |
| `0x53` | 0                  | map type-hint               |
| `0x54` | `l16 n16 kt ps...` | hashset value-random index  |
| `0x55` | `l16 n16 kt ps...` | hashset value-ordered index |
| `0x56` | `l16 n16 kt ps...` | ordset value-random index   |
| `0x57` | `l16 n16 kt ps...` | ordset value-ordered index  |
| `0x58` | `l16 n16 kt ps...` | hashkey value-random index  |
| `0x59` | `l16 n16 kt ps...` | hashkey value-ordered index |
| `0x5a` | `l16 n16 kt ps...` | ordkey value-random index   |
| `0x5b` | `l16 n16 kt ps...` | ordkey value-ordered index  |
| `0x5c` | `l16 n16 kt ps...` | hashval value-random index  |
| `0x5d` | `l16 n16 kt ps...` | hashval value-ordered index |
| `0x5e` | `l16 n16 kt ps...` | ordval value-random index   |
| `0x5f` | `l16 n16 kt ps...` | ordval value-ordered index  |
| `0x64` | `l32 n32 kt ps...` | hashset value-random index  |
| `0x65` | `l32 n32 kt ps...` | hashset value-ordered index |
| `0x66` | `l32 n32 kt ps...` | ordset value-random index   |
| `0x67` | `l32 n32 kt ps...` | ordset value-ordered index  |
| `0x68` | `l32 n32 kt ps...` | hashkey value-random index  |
| `0x69` | `l32 n32 kt ps...` | hashkey value-ordered index |
| `0x6a` | `l32 n32 kt ps...` | ordkey value-random index   |
| `0x6b` | `l32 n32 kt ps...` | ordkey value-ordered index  |
| `0x6c` | `l32 n32 kt ps...` | hashval value-random index  |
| `0x6d` | `l32 n32 kt ps...` | hashval value-ordered index |
| `0x6e` | `l32 n32 kt ps...` | ordval value-random index   |
| `0x6f` | `l32 n32 kt ps...` | ordval value-ordered index  |
| `0x74` | `l64 n64 kt ps...` | hashset value-random index  |
| `0x75` | `l64 n64 kt ps...` | hashset value-ordered index |
| `0x76` | `l64 n64 kt ps...` | ordset value-random index   |
| `0x77` | `l64 n64 kt ps...` | ordset value-ordered index  |
| `0x78` | `l64 n64 kt ps...` | hashkey value-random index  |
| `0x79` | `l64 n64 kt ps...` | hashkey value-ordered index |
| `0x7a` | `l64 n64 kt ps...` | ordkey value-random index   |
| `0x7b` | `l64 n64 kt ps...` | ordkey value-ordered index  |
| `0x7c` | `l64 n64 kt ps...` | hashval value-random index  |
| `0x7d` | `l64 n64 kt ps...` | hashval value-ordered index |
| `0x7e` | `l64 n64 kt ps...` | ordval value-random index   |
| `0x7f` | `l64 n64 kt ps...` | ordval value-ordered index  |

**NOTE:** the size of the index target subscripts is the same as the size of the length -- that is, `l16` uses 16-bit unsigned ints to index into the data structure.

**NOTE:** `l16`, `l32`, and `l64` encode `len(kt) + len(ps...)` -- that is, the key type is included. This avoids typecode parsing overhead when we want to skip over the index.

Each element offset is encoded in the same number of bits as the index length, so for `0x5N` indexes the element offset is `uint16`; for `0x7N` it would be `uint64`.

Position indexes translate `[i]` subscripts to byte-offsets within a tuple. They can be downsampled by a number of bits, encoded as an `int8`; for example, if `bits = 2`, then the byte-offset table encodes the positions of `[0]`, `[4]`, `[8]`, `[12]`, etc. This trades space for time.

**NOTE:** the index's target values refer to the target base plus the exact element offset; there is no header stuff that you need to think about when fetching the element. This minimizes the likelihood of cache misses when using indexes to fetch things.

`ord*` indexes need not be present in the original collection; the purpose is to provide interpolation points against the query space.


## Transit spec
A lightweight framing protocol that wraps one or more values over non-persistent connections, e.g. pipes or networks. There is no integrity verification or alignment.

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
Frames are 16-byte aligned and begin with magic+length, the boundary value, the frame key, and the signed frame key.

```cpp
struct utf9_disk_frame_header
{
  uint8_t const magic[8]   = {0xff, 0xff, 'u', 't', 'f', '9', 'f', 0x00};
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
  uint8_t const          padding[header.length + 15 & ~15] = {0};
};
```

`sizeof(utf9_frame_header) == 80`. Because frames carry the header overhead, each frame can contain multiple values emitted back to back. The frame is there to provide integrity checking for its contents and to provide a seek point within the file, making it possible to jump to an arbitrary location and find a nearby frame boundary.

`boundary` decreases the amount of overhead required to scan for a frame boundary, while `frame_sha` and `signed_sha` provide robust confirmation.
