# Frames
A frame is a single chunk of data for which atomic transit is guaranteed: that is, each pipeline element is able to load the whole frame into memory.


## Use cases
1. Frames carry _all_ data between pipeline elements
2. Frames are sometimes keyed
3. Frames are sometimes sorted by one or more key fields
4. Frames carry _τ_ sequence markers for specified fields
5. Frames will often acquire or lose a multiplexing prefix key-field
6. Fields, especially the first ones, should be accessible without decoding the rest of the frame (the "decoding minimality principle")

From the outside:

1. Frames must be atomically manageable with minimal decoding (i.e. `splice` should work, so we need a fixed-offset `length` field)
2. It should be possible to IO a frame using `readv` and `writev`, to minimize copies required to assemble a frame in RAM


## Field types
**TODO:** revise this section

1. _τ_ marker
2. Byte/numeric array, ordered by bytes
3. `msgpack` data structure, not ordered _but length-prefixed_

_τ_ markers have no data associated with them.

Arrays are dense and encode their dimensions as a variable-length zero-terminated prefix on the value. Each dimension is a `uint32_t` and the final dimension is always zero. For a scalar, the dimension is just `0`.

`msgpack` data is prefixed by `uint32_t length`.

**Q:** should we prefer `msgpack` for every field? Then it's easy, and we can use `ext` values for packed arrays. (Downside: `msgpack` values aren't binary-comparable.)


## _τ_ stream markers
_τ_ can be either hierarchical or parallel -- but do we want to think about zipped parallel orderings? I'm thinking perhaps not; we'd almost always demultiplex first and _τ_-cycle each separately.

So the natural way for _τ_ to work is that if field _n_ is _τ_, then fields _n+1.._ are also _τ_. That is, _τ_ propagation follows key ordering; this implies a few things:

1. Keys have a consistent ordering at the _frame_ level (not just semantic)
2. _τ_ encoding follows frame field ordering
3. Every stream is strictly hierarchical; zip requires a keyed multiplex, not zipped _τ_
4. `memcmp` follows `writev` layout
5. Key fields must be `byte[]`, not `msgpack` (via memcmp, via (4), unless we want decoding overhead)

**Q:** are these good requirements?


## Structure
```cpp
struct utf9_frame
{
  uint8_t    magic[4] = {0xff, 0xff, 'u', '9'};
  uint32_t   length;     // implied by outside (1) above

  uint32_t   n;          // number of fields (if 0, τ for the whole stream)
  uint32_t   starts[n];  // start byte of each field (rel to &magic[0])
  field_type fields[n];  // type metadata for each field
  uint8_t    data[length - starts[n - 1]];
};


enum field_type          // assume this is bit-packed efficiently
{
  TAU,
  SAME,
  BYTES,
  ARRAY,
  MSGPACK,
  KEY;                   // short < long, memcmp() for same-length
};
```
