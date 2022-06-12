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

Lazy loading means that a `utf9` record view must store a diff in order to provide mutability. This diff can have a ravel, which means we can stream values into a `utf9` diff reduction to construct a final object (emitted on _τ_). This model unifies diff-streaming and atomic record edits, providing a nice scaffold for later features like OT -- nothing stops us from defining an OT diff reducer that reorders edits on user-submitted time and emits the reconciled state.

In other words, [row transformation](transform.md) amounts to generating modification ops into a suitable reduction context. That makes row transforms differentiable by time.
