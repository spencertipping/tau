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
1. _τ_ marker
2. Byte/numeric array, ordered by bytes
3. `msgpack` data structure, not ordered _but length-prefixed_

_τ_ markers have no data associated with them.

Arrays are dense and encode their dimensions as a variable-length zero-terminated prefix on the value. Each dimension is a `uint32_t` and the final dimension is always zero. For a scalar, the dimension is just `0`.

`msgpack` data is prefixed by `uint32_t length`.


## Structure
```c
struct utf9_frame
{
  uint8_t  magic[4] = {0xff, 0xff, 'u', '9'};
  uint32_t length;
  // ...
};
```
