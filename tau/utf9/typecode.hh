#ifndef tau_utf9_typecode_h
#define tau_utf9_typecode_h


#include "../types.hh"

#include "../module/begin.hh"


namespace tau::utf9
{


enum val_type : u8
{
  UINT8    = 0x00,
  UINT16   = 0x01,
  UINT32   = 0x02,
  UINT64   = 0x03,
  INT8     = 0x04,
  INT16    = 0x05,
  INT32    = 0x06,
  INT64    = 0x07,
  FLOAT32  = 0x08,
  FLOAT64  = 0x09,
  SYMBOL   = 0x0a,
  PIDFD    = 0x0b,
  BOOL     = 0x0c,
  NULLTYPE = 0x0e,

  GREEK    = 0x10,

  UTF8     = 0x18,
  BYTES    = 0x1c,

  TUPLE    = 0x20,
  ARRAY    = 0x24,
  LIST     = 0x30,  // indexed tuple variants
  SET      = 0x31,
  MAP      = 0x32,

  NONE     = 0x3e,  // fictitious: not a real value
  BOGUS    = 0x3f,

  MAX_VAL_TYPE = BOGUS,
};


typedef u64 val_type_mask;

static_assert(MAX_VAL_TYPE < sizeof(val_type_mask) * 8);


namespace  // Type dispatch tables
{

val_type const constexpr bts[256] =
{
  // 0x00-0x0f
  UINT8, UINT16, UINT32, UINT64,
  INT8,  INT16,  INT32,  INT64,
  FLOAT32, FLOAT64, SYMBOL, PIDFD,
  BOOL, BOOL, NULLTYPE, BOGUS,

  // 0x10-0x1f
  GREEK, GREEK, GREEK, GREEK,
  GREEK, GREEK, GREEK, BOGUS,
  UTF8, UTF8, UTF8, UTF8,
  BYTES, BYTES, BYTES, BYTES,

  // 0x20-0x2f
  UTF8, UTF8, UTF8, UTF8,
  UTF8, UTF8, UTF8, UTF8,
  UTF8, UTF8, UTF8, UTF8,
  UTF8, UTF8, UTF8, UTF8,

  // 0x30-0x3f
  BYTES, BYTES, BYTES, BYTES,
  BYTES, BYTES, BYTES, BYTES,
  BYTES, BYTES, BYTES, BYTES,
  BYTES, BYTES, BYTES, BYTES,

  // 0x40-0x4f
  TUPLE, TUPLE, TUPLE, TUPLE,
  ARRAY, ARRAY, ARRAY, ARRAY,
  TUPLE, TUPLE, TUPLE, TUPLE,
  TUPLE, TUPLE, TUPLE, TUPLE,

  // 0x50-0x5f
  LIST,  LIST,  LIST,  LIST,
  SET,   SET,   SET,   SET,
  MAP,   MAP,   MAP,   MAP,
  LIST,  MAP,   SET,   BOGUS,

  // 0x60-0x6f
  BOGUS, BOGUS, BOGUS, BOGUS,
  BOGUS, BOGUS, BOGUS, BOGUS,
  BOGUS, BOGUS, BOGUS, BOGUS,
  BOGUS, BOGUS, BOGUS, BOGUS,

  // 0x70-0x7f
  BOGUS, BOGUS, BOGUS, BOGUS,
  BOGUS, BOGUS, BOGUS, BOGUS,
  BOGUS, BOGUS, BOGUS, BOGUS,
  BOGUS, BOGUS, BOGUS, BOGUS,

  // 0x80-0xbf
  INT8, INT8, INT8, INT8,
  INT8, INT8, INT8, INT8,
  INT8, INT8, INT8, INT8,
  INT8, INT8, INT8, INT8,

  INT8, INT8, INT8, INT8,
  INT8, INT8, INT8, INT8,
  INT8, INT8, INT8, INT8,
  INT8, INT8, INT8, INT8,

  INT8, INT8, INT8, INT8,
  INT8, INT8, INT8, INT8,
  INT8, INT8, INT8, INT8,
  INT8, INT8, INT8, INT8,

  INT8, INT8, INT8, INT8,
  INT8, INT8, INT8, INT8,
  INT8, INT8, INT8, INT8,
  INT8, INT8, INT8, INT8,

  // 0xc0-0xff
  INT8, INT8, INT8, INT8,
  INT8, INT8, INT8, INT8,
  INT8, INT8, INT8, INT8,
  INT8, INT8, INT8, INT8,

  INT8, INT8, INT8, INT8,
  INT8, INT8, INT8, INT8,
  INT8, INT8, INT8, INT8,
  INT8, INT8, INT8, INT8,

  INT8, INT8, INT8, INT8,
  INT8, INT8, INT8, INT8,
  INT8, INT8, INT8, INT8,
  INT8, INT8, INT8, INT8,

  INT8, INT8, INT8, INT8,
  INT8, INT8, INT8, INT8,
  INT8, INT8, INT8, INT8,
  INT8, INT8, INT8, INT8,
};

}


static_assert(bts[0x7f] != INT8);
static_assert(bts[0x80] == INT8);


}


#include "../module/end.hh"

#endif
