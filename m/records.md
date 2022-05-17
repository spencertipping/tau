# Frames
A frame is a single chunk of data for which atomic transit is guaranteed: that is, each pipeline element is able to load the whole frame into memory.

Frames can have multiple fields. Typically, fields are prefixed to form a multiplexing or key space; then the assumption is that each sub-stream has its own state.

_τ_, _α_, and _ω_ are meta-markers that manipulate stream state. _τ_ means "time has cycled", which primarily impacts orderings. In particular, it means "the key ordering for this field and rightward is now reset". _ω_ is an error/EOF marker that means "time has ended and will never resume" -- i.e. the supplier of the stream has disconnected and will not be coming back. _α_ is a "time begins now" message sent by newly-connected stream elements.


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


## Structure
```cpp
struct frame             // a utf9 compact frame (meant for memory, not disk)
{
  uint8_t    magic[4] = {0xff, 0xff, 'u', '9'};
  uint32_t   length;     // implied by outside (1) above
  int32_t    n;          // number of fields
    // if  0, τ for the stream
    // if -1, ω for the stream
  uint32_t   starts[n];  // start byte of each field (rel to &magic[0])
  field_type fields[n];  // type metadata for each field
  uint8_t    data[length - starts[0]];
};


enum field_type          // assume this is bit-packed efficiently
{
  // metadata
  TAU,                   // cyclic tau marker
  ALPHA,                 // acyclic start marker
  OMEGA,                 // acyclic end marker

  // data
  FD,                    // file descriptors must be sent specially
  BYTES,                 // bytes of unspecified structure
  ARRAY,                 // dense numeric array
  MSGPACK,               // msgpack-encoded object of unspecified structure
  KEY;                   // lexically-ordered byte array key
};
```
