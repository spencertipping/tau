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


## Structure
```cpp
struct utf9_compact_frame
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
  OMEGA,                 // acyclic end marker
  FD,                    // file descriptors must be sent specially

  // data
  BYTES,                 // bytes of unspecified structure
  ARRAY,                 // dense numeric array
  MSGPACK,               // msgpack-encoded object of unspecified structure
  KEY;                   // lexically-ordered byte array key
};
```
