# UTF9 records
Key characteristics of records:

1. Efficient to serialize/deserialize, both in space and time
2. Able to hold large (>1TiB) amounts of data
3. Efficient to left-cons and left-match fields, e.g. for multiplexing
4. Easy to encode fields of commonly-used datatypes
5. Fast C++ API for tau record fields
6. Side-channel symbols: _α_, _ω_, _ι_, _κ_, and _τ(x)_


## Meta-symbols
+ _α_ is a "time begins now" message sent by newly-connected stream elements
+ _ω_ is an error/EOF marker that means "time has ended and will never resume"
+ _ι_ means "you can start sending data now"
+ _κ_ means "you cannot send more data now; it will block"
+ _τ_ means "time has cycled", which primarily impacts orderings
  + _τ_ can also mean "you are at this point within a cycle", which can be used for optimization
  + Partial _τ_ is always considered to be an estimate, and it is legal to rewind _τ_ values to revise these estimates


## Core data types
+ Fixed-size
  + `fd`: file descriptor (handled specially across local IPC boundaries)
  + `int8`, `int16`, `int32`, `int64` (operators are signed/unsigned, ints are just bits -- we have all sizes for array purposes)
  + `float32`, `float64`
  + SSE/AVX?
  + `symbol` (for stuff like `nil`, `true`, `false`, and custom)
  + `[]` packed array of fixed-size things
  + `()` tuple of fixed-size things
+ Variable-size
  + `utf8` text
  + `bytes` opaque blob
  + `[]` (n-dimensional homogeneous array)
  + `()` (one-dimensional heterogeneous tuple)
  + `{}` associative map
  + `{}` set

**NOTE:** maps/sets are just tuples or arrays that have an index attached, e.g. for hashed lookup. The index is a preloaded scan of the ravel, meaning that these types are ordered either strictly, or by element hashes.

**NOTE:** fds can be moved with https://www.man7.org/linux/man-pages/man2/pidfd_getfd.2.html, so we don't strictly need a domain socket.


### Ravel-scan indexes
Because indexes scan against the container ravel, they don't have to be used strictly for a single purpose like associative or hashed lookup. We can build skip lists, tree-like data structures, and pretty much anything else you might want to look up in a nonlinear way -- all using the same index.

I think we can get away with two types of indexes:

1. Arbitrarily-ordered quantile positions
2. Uniformly-distributed radix points

(1) enables binary-search, (2) enables very accurate interpolation search, likely _< O(log log n)_.

Naturally, these indexes can be generated in streaming form, and they rely on _τ(i)_ estimates to size themselves.


## Transit spec
`utf9` is a `msgpack`-inspired format that differs in a few ways:

+ `utf9` prepends a header
+ 64-bit lengths are allowed
+ Nontrivial containers (arrays and maps) include their internal complexity and encoded length in bytes
+ Packed arrays are first-class (not just `bin` and `ext`)
+ `utf9` adds the `set` datatype
+ `utf9` adds the `symbol` datatype
+ `utf9` adds _α_, _ω_, ... meta-symbols

`utf9` is designed to be usable in-place, that is without "unpacking" into an in-memory datastructure. This means a few things:

1. Containers within `utf9` records are lazily loaded, and refer to the original record rather than decoding into a temporary data structure
2. Containers prepend byte-lengths, indexes, and complexity measures
3. Functionally speaking, `utf9` behaves like a micro-heap that is GC'd prior to serialization

Note that if we use a bytecode format like `msgpack`, our value-type can just be a pointer to the beginning byte offset. If we want diffs, we can use two pointers (one to the diff, one to the original). That's very compact; I like it.

Lazy loading means that a `utf9` record view must store a diff in order to provide mutability. This diff can have a ravel, which means we can stream values into a `utf9` diff reduction to construct a final object (emitted on _τ_). This model unifies diff-streaming and atomic record edits, providing a nice scaffold for later features like OT -- nothing stops us from defining an OT diff reducer that reorders edits on user-submitted time and emits the reconciled state.

In other words, [row transformation](transform.md) amounts to generating modification ops into a suitable reduction context. That makes row transforms differentiable by time. It also means we can reuse all of the row-transformation operations as streaming things, with _τ_ markers to emit the current state.

**NOTE:** this brings delta operators like `(+ 5)` and map-insert-and-merge front and center. We should have a broad dictionary of these things to use within row context.

**NOTE:** `utf9` records may be aliased in memory, in which case they shouldn't be modified; then all updates will be applied as diff patches.


### Bytecode design
Bytecode-encoded objects need not point to the origin; that is, there are no parent links (just like `msgpack`). Each object must be decodable with no base-pointer context, meaning we don't keep a base pointer and there is no value aliasing.

Modifications are also bytecodes of a sort, but indirectly encoded using symbols.

Like `msgpack`, we optimize for brevity by providing `fix*` variants for small structures.


### Atomic types
| Byte | Following bytes | Description                  |
|------|-----------------|------------------------------|
|      | 1               | `int8`                       |
|      | 2               | `int16`                      |
|      | 4               | `int32`                      |
|      | 8               | `int64`                      |
|      | 4               | `float32`                    |
|      | 8               | `float64`                    |
|      | 8               | `symbol`                     |
|      | 4               | `fd`                         |
|      | 1 + len         | `utf8`, 8-bit _byte_ length  |
|      | 2 + len         | `utf8`, 16-bit _byte_ length |
|      | 4 + len         | `utf8`, 32-bit _byte_ length |
|      | 8 + len         | `utf8`, 64-bit _byte_ length |
|      | 1 + len         | `bytes`, 8-bit length        |
|      | 2 + len         | `bytes`, 16-bit length       |
|      | 4 + len         | `bytes`, 32-bit length       |
|      | 8 + len         | `bytes`, 64-bit length       |
|      | 0               | `fixint`                     |
|      | 0               | `fixfloat`                   |
|      | len             | `fixstr`                     |
|      | len             | `fixbytes`                   |
|      | len             | `fixnop`                     |

Note that both `utf8` and `bytes` encode the _byte_ length of the payload, not the logical number of elements as you might expect. This makes it possible to skip over the value without running a UTF-8 decode loop.


### Simple containers
| Byte | Following bytes | Description                                        |
|------|-----------------|----------------------------------------------------|
|      | `l n xs...`     | `tuple` of byte-length `int8 l`,  length `int8 n`  |
|      | `l n xs...`     | `tuple` of byte-length `int16 l`, length `int16 n` |
|      | `l n xs...`     | `tuple` of byte-length `int32 l`, length `int32 n` |
|      | `l n xs...`     | `tuple` of byte-length `int64 l`, length `int64 n` |
|      | `t n xs...`     | `array` of type `t`,  length `int8 n`              |
|      | `t n xs...`     | `array` of type `t`,  length `int16 n`             |
|      | `t n xs...`     | `array` of type `t`,  length `int32 n`             |
|      | `t n xs...`     | `array` of type `t`,  length `int64 n`             |
|      | `l xs...`       | `fixtuple` of `n` elements, byte-length `int8 l`   |
|      | `l xs...`       | `fixtuple` of `n` elements, byte-length `int16 l`  |

Arrays distribute a type prefix across a series of elements; for example, `array int8 5` would then be followed by five bytes, each of which would be interpreted as though it had been specified with `int8`. The type prefix `t` need not be a single byte; you can have `array utf8 5 5` for an array of five-byte UTF-8 strings. You can also have arrays within arrays.

Array types always include the byte length and never include the value. That is, they end at the length-specification. Examples of array element types:

+ `tuple 48`
+ `array int8 4`
+ `fixtuple4 48`
+ `utf8 10`
+ `array array fixbytes(10) 10 10` -- that is, a 10x10 array of 10-long `bytes` objects


### Container indexes
Container indexes modify simple containers by prepending an index that provides fast lookups. The container's values are usually sorted by the indexed field for content-indexed lookups. Large heterogeneous tuples can be indexed by element subscripts. All indexes map some aspect of the data to the byte offset of the container element that contains it.

Indexes can be complete (`cidx`) or incomplete (`iidx`). Complete indexes provide a mapping for every element, incomplete indexes don't. Incomplete indexes enable fast interpolation search, complete indexes allow elements to be found even when they aren't in sorted order. Unordered elements must be addressed with a complete index; otherwise an index miss would result in a full linear scan.

Indexes are tuples of `(ks, ka, ia)`. `ks` is a keyspec, which specifies which value is being indexed. `ka` and `ia` are the array of keys and the array of seek-indexes, respectively.

| Byte | Following bytes | Description                       |
|------|-----------------|-----------------------------------|
|      | `l ks ka ia`    | `cidx` with byte-length `int16 l` |
|      | `l ks ka ia`    | `cidx` with byte-length `int32 l` |
|      | `l ks ka ia`    | `cidx` with byte-length `int64 l` |
|      | `l ks ka ia`    | `iidx` with byte-length `int16 l` |
|      | `l ks ka ia`    | `iidx` with byte-length `int32 l` |
|      | `l ks ka ia`    | `iidx` with byte-length `int64 l` |

The data structure being indexed occurs immediately after the index. Logically, it's treated as a part of the index itself (which yields "a map" or "a set") but the two are decoded independently.

**NOTE:** multiple indexes can be stacked. For example, we can have an associative map of `(k v)` tuples that is indexed both by `i` numerical indexes and `k` ordering.


#### Keyspec
The keyspec is a symbol that describes the aspect of the data that's being indexed. It is currently one of:

+ For whole elements, e.g. sets
  + `i`: the element position
  + `h`: `hash(element)`, possibly truncated to just the top _n_ bits
+ For `(k v₁ v₂ ...)` associative tuples
  + `k`: the initial element value
  + `kh`: `hash(element[0])`, possibly truncated to the just the top _n_ bits

**TODO:** add further index capability, e.g. `x[i]` for custom `i`

The index type influences the idiomatic data structure preference:

+ `i`: array
+ `h`: set
+ `k` and `kh`: associative map/multimap


#### Seek arrays
`ka` is an array of fixed-size keys and `ia` is an array of fixed-size seek offsets (relative to the container base). `ka` is always ordered, but _`ia` need not be forward-ordered for complete indexes._ This allows container tuples to act like databases, providing multiple parallel indexes for the same data.

`ka` can have any type as long as an ordering exists. `ia` is always `int16`, `int32`, or `int64`.


## Disk spec
Similar to the transit spec, but intended for long-term persistence and easy lookup. In particular, we want a few features to work well from an IO perspective:

1. File splitting: seek to somewhere in the middle and find record boundaries
2. Optional robustness checking, both per-record and per-file
3. Fast record skipping for the sequential-access case
4. Preallocated space
5. File-level indexes
