# Records
Records have fields, each of which has a type that describes its codec.

_τ_, _α_, _ω_, _ι_, and _κ_ are meta-markers that manipulate stream state:

+ _α_ is a "time begins now" message sent by newly-connected stream elements
+ _ω_ is an error/EOF marker that means "time has ended and will never resume"
+ _ι_ means "you can start sending data now"
+ _κ_ means "you cannot send more data now; it will block"
+ _τ_ means "time has cycled", which primarily impacts orderings
  + _τ_ can also mean "you are at this point within a cycle", which can be used for optimization
  + Partial _τ_ is always considered to be an estimate, and it is legal to rewind _τ_ values to revise these estimates


## Structure
A record is a logical row of data, with independently-decodable fields. Within a multiplexed stream, key fields will be leftwards of data fields, and it's often sufficient to look up key fields without decoding the others (e.g. to demultiplex a stream).

Note that _τ_ markers are unlike _α_ and _ω_ in that _τ_ maps to a 64-bit value that represents the approximate fraction of the stream that has been sent so far. Symbolic _τ_ reset is assumed when the value is 0 (i.e. beginning of a new cycle).

**Q:** what's the purpose of having a large number of fields, when we have `msgpack` structures that are better-suited to most domain-specific values?

```cpp
struct record            // a utf9 compact record (meant for memory, not disk)
{
  uint8_t  magic[4] = {0xff, 'u', '9', 0x00};
  uint48_t length;       // up to 256TiB
  uint16_t n;            // number of fields
  uint64_t fields[n];    // each is start_offset << 16 | field_type
  uint8_t  data[length - (fields[0] >> 16)];
};

enum field_type          // assume this is bit-packed efficiently
{
  // stream operators
  ALPHA,                 // acyclic start marker
  OMEGA,                 // acyclic end marker
  IOTA,                  // flow-start impulse (edge trigger)
  KAPPA,                 // flow-stop impulse (edge trigger)
  TAU,                   // cyclic tau marker

  // data elements
  FDS,                   // array of file descriptors (special IPC)
  BYTES,                 // bytes of unspecified structure, lex-ordered
  MSGPACK,               // msgpack-encoded object of unspecified structure
  ARRAY;                 // dense numeric array
};
```

`sizeof(record) == record.length`; that is, `.length` is an outer measurement.

The final field is `length - (fields[n-1] >> 16)` bytes long.

Toplevel stream control is encoded with a one-field record; that field is a stream operator like `ALPHA` or `IOTA`.


## Field encodings
+ `ALPHA`, `OMEGA`, `IOTA`, and `KAPPA` have no value
+ `TAU` corresponds to a single 8-byte field value


### `FDS`
File descriptors are packed into a `uint32_t[]` binary array in native endian format. Since the array consists of fixed-size members, it has no length prefix.


### `BYTES`
Byte arrays are encoded verbatim with no length prefix.


### `MSGPACK`
[msgpack](https://msgpack.org) payload with no prefix or other wrapping.


### `ARRAY`
Numeric arrays specify both the element size/type and the array shape. The array is dense, so the shape is encoded as a list of dimensions.

```cpp
struct array
{
  array_type etype;      // type of each element
  uint8_t    n;          // number of dimensions
  uint64_t   dims[n];
  T          elems[prod(dims)];
};

enum array_type
{
  INT8,    UINT8,
  INT16,   UINT16,
  INT32,   UINT32,
  INT64,   UINT64,
  FLOAT32, FLOAT64;
};
```

Dimensions are lex-ordered: the first dimension is the last to change as you walk forwards through array elements.

Array elements are always stored with native endianness.
