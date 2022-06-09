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

**TODO:** do we want associative structures, or do we want indexed tuples? Indexes are probably much more versatile.

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

`utf9` is designed to be used in-place, that is without "unpacking" into an in-memory datastructure. This means a few things:

1. `utf9` structures are shape-immutable
2. Containers prepend byte-lengths, indexes, and complexity measures
3. Functionally speaking, `utf9` behaves like a micro-heap that is GC'd prior to serialization

**TODO:** explore the micro-heap idea. It makes a lot of sense, and we can make calculated tradeoffs between "GC first then send" vs "send without GC" to trade space and time

**NOTE:** immutable modification patches are very much like OT modifications; there may be a way to generalize the "apply edits to a thing" logic as a form of GC
