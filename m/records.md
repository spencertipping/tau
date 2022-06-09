# Records
Key characteristics of records:

1. Efficient to serialize/deserialize, both in space and time
2. Able to hold large (>1TiB) amounts of data
3. Efficient to left-cons and left-match fields, e.g. for multiplexing
4. Easy to encode fields of commonly-used datatypes
5. Fast C++ API for tau record fields
6. Side-channel symbols: _α_, _ω_, _ι_, _κ_, and _τ(x)_

**TODO:** define records with respect to `msgpack`; I believe we can fully embed our format (note that `msgpack` does have the embedding property required for (3))


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
  + `int8`, `int16`, `int32`, `int64` (operators are signed/unsigned, ints are just bits)
  + `float32`, `float64`
  + `symbol` (for stuff like `nil`, `true`, and `false`)
  + `[]` (n-dimensional homogeneous array of fixed-type things)
  + `()` (one-dimensional heterogeneous tuple of fixed-size things)
+ Variable-size
  + `utf8` text
  + `{}` associative map
