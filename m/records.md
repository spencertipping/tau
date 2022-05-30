# Records
Records have fields, each of which has a type that describes its codec.

_τ_, _α_, and _ω_ are meta-markers that manipulate stream state. _τ_ means "time has cycled", which primarily impacts orderings. In particular, it means "the key ordering for this field and rightward is now reset". _ω_ is an error/EOF marker that means "time has ended and will never resume" -- i.e. the supplier of the stream has disconnected and will not be coming back. _α_ is a "time begins now" message sent by newly-connected stream elements.

User operators can synthesize _τ_, _α_, and _ω_. By convention, a record will end with at most one of these three fields, and they appear only in the final (rightmost) position. This is a consequence of lexical ordering. You can produce records that work differently, but they will have undefined behavior with standard operators.


## Structure
A record is a logical row of data, with independently-decodable fields. Within a multiplexed stream, key fields will be leftwards of data fields, and it's often sufficient to look up key fields without decoding the others (e.g. to demultiplex a stream).

Note that _τ_ markers are unlike _α_ and _ω_ in that _τ_ maps to a 64-bit value that represents the approximate fraction of the stream that has been sent so far. Symbolic _τ_ reset is assumed when the value is 0 (i.e. beginning of a new cycle).

```cpp
struct record            // a utf9 compact record (meant for memory, not disk)
{
  uint8_t    magic[4] = {0xff, 'u', '9', 0x00};
  uint48_t   length;     // implied by outside (1) above
  uint16_t   n;          // number of fields
  uint32_t   starts[n];  // start byte of each field (rel to &magic[0])
  field_type fields[shape > 0 ? 0 : n];  // type metadata for each field
  uint8_t    data[length - starts[0]];
};
```

**TODO:** I don't love having explicit field offsets for everything, because they'll take up a lot of space for small records. There should be a way to encode `field_type` and size information in the same place.

**TODO:** `starts[]` must provide enough bits to encode offsets up to the full length; right now we're 16 bits short

`uint48_t` isn't a real type, but the length is encoded with unsigned 48 bits, allowing records up to 256TiB.

```cpp
enum field_type          // assume this is bit-packed efficiently
{
  // stream operators
  TAU,                   // cyclic tau marker
  ALPHA,                 // acyclic start marker
  OMEGA,                 // acyclic end marker
  IOTA,                  // flow-start impulse (edge trigger)

  // data elements
  FDS,                   // array of file descriptors (special IPC)
  BYTES,                 // bytes of unspecified structure
  ARRAY,                 // dense numeric array
  MSGPACK;               // msgpack-encoded object of unspecified structure
};
```

The final field is `length - starts[n - 1]` bytes long.

Toplevel stream control is encoded with a one-field record whose field has type `TAU`, `ALPHA`, `OMEGA`, or `IOTA`.
