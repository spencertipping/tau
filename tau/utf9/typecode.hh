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

  UINT     = 0x3c,  // auto-sized ints (not real, just for API convenience)
  INT      = 0x3d,

  NONE     = 0x3e,  // fictitious: not a real value
  BOGUS    = 0x3f,

  MAX_VAL_TYPE = BOGUS,
};


typedef u64 val_type_mask;

static_assert(MAX_VAL_TYPE < sizeof(val_type_mask) * 8);


val_type_mask const constexpr uint_types   = 1ull << UINT | 1ull << UINT64 | 1ull << UINT32 | 1ull << UINT16 | 1ull << UINT8;
val_type_mask const constexpr int_types    = 1ull <<  INT | 1ull <<  INT64 | 1ull <<  INT32 | 1ull <<  INT16 | 1ull <<  INT8;
val_type_mask const constexpr float_types  = 1ull << FLOAT32 | 1ul << FLOAT64;
val_type_mask const constexpr number_types = uint_types | int_types | float_types;
val_type_mask const constexpr string_types = 1ull << UTF8 | 1ull << BYTES;
val_type_mask const constexpr multi_types  = 1ull << TUPLE | 1ull << ARRAY | 1ull << LIST | 1ull << SET | 1ull << MAP;


namespace  // Type dispatch tables
{


// Type mappings for bytecode values
val_type const constexpr vts[256] =
{
  // 0x00-0x0f
  UINT, UINT, UINT, UINT,
  INT,  INT,  INT,  INT,
  FLOAT32, FLOAT64, SYMBOL, PIDFD,
  BOOL, BOOL, NULLTYPE, BOGUS,

  // 0x10-0x1f
  GREEK, GREEK, GREEK, GREEK,
  GREEK, GREEK, GREEK, BOGUS,
  UTF8,  UTF8,  UTF8,  UTF8,
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
  INT, INT, INT, INT,
  INT, INT, INT, INT,
  INT, INT, INT, INT,
  INT, INT, INT, INT,

  INT, INT, INT, INT,
  INT, INT, INT, INT,
  INT, INT, INT, INT,
  INT, INT, INT, INT,

  INT, INT, INT, INT,
  INT, INT, INT, INT,
  INT, INT, INT, INT,
  INT, INT, INT, INT,

  INT, INT, INT, INT,
  INT, INT, INT, INT,
  INT, INT, INT, INT,
  INT, INT, INT, INT,

  // 0xc0-0xff
  INT, INT, INT, INT,
  INT, INT, INT, INT,
  INT, INT, INT, INT,
  INT, INT, INT, INT,

  INT, INT, INT, INT,
  INT, INT, INT, INT,
  INT, INT, INT, INT,
  INT, INT, INT, INT,

  INT, INT, INT, INT,
  INT, INT, INT, INT,
  INT, INT, INT, INT,
  INT, INT, INT, INT,

  INT, INT, INT, INT,
  INT, INT, INT, INT,
  INT, INT, INT, INT,
  INT, INT, INT, INT,
};


// Type mappings for typecode values
val_type const constexpr tts[256] =
{
  // 0x00-0x0f
  UINT8, UINT16, UINT32, UINT64,
  INT8,  INT16,  INT32,  INT64,
  FLOAT32, FLOAT64, SYMBOL, PIDFD,
  BOOL, BOOL, BOGUS, BOGUS,

  // 0x10-0x1f
  GREEK, GREEK, GREEK, GREEK,
  GREEK, GREEK, GREEK, BOGUS,
  UTF8,  UTF8,  UTF8,  UTF8,
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
  BOGUS, BOGUS, BOGUS, BOGUS,
  BOGUS, BOGUS, BOGUS, BOGUS,
  BOGUS, BOGUS, BOGUS, BOGUS,
  BOGUS, BOGUS, BOGUS, BOGUS,

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
  BOGUS, BOGUS, BOGUS, BOGUS,
  BOGUS, BOGUS, BOGUS, BOGUS,
  BOGUS, BOGUS, BOGUS, BOGUS,
  BOGUS, BOGUS, BOGUS, BOGUS,

  BOGUS, BOGUS, BOGUS, BOGUS,
  BOGUS, BOGUS, BOGUS, BOGUS,
  BOGUS, BOGUS, BOGUS, BOGUS,
  BOGUS, BOGUS, BOGUS, BOGUS,

  BOGUS, BOGUS, BOGUS, BOGUS,
  BOGUS, BOGUS, BOGUS, BOGUS,
  BOGUS, BOGUS, BOGUS, BOGUS,
  BOGUS, BOGUS, BOGUS, BOGUS,

  BOGUS, BOGUS, BOGUS, BOGUS,
  BOGUS, BOGUS, BOGUS, BOGUS,
  BOGUS, BOGUS, BOGUS, BOGUS,
  BOGUS, BOGUS, BOGUS, BOGUS,

  // 0xc0-0xff
  BOGUS, BOGUS, BOGUS, BOGUS,
  BOGUS, BOGUS, BOGUS, BOGUS,
  BOGUS, BOGUS, BOGUS, BOGUS,
  BOGUS, BOGUS, BOGUS, BOGUS,

  BOGUS, BOGUS, BOGUS, BOGUS,
  BOGUS, BOGUS, BOGUS, BOGUS,
  BOGUS, BOGUS, BOGUS, BOGUS,
  BOGUS, BOGUS, BOGUS, BOGUS,

  BOGUS, BOGUS, BOGUS, BOGUS,
  BOGUS, BOGUS, BOGUS, BOGUS,
  BOGUS, BOGUS, BOGUS, BOGUS,
  BOGUS, BOGUS, BOGUS, BOGUS,

  BOGUS, BOGUS, BOGUS, BOGUS,
  BOGUS, BOGUS, BOGUS, BOGUS,
  BOGUS, BOGUS, BOGUS, BOGUS,
  BOGUS, BOGUS, BOGUS, BOGUS,
};

}


static_assert(vts[0x7f] != INT);
static_assert(vts[0x80] == INT);


}


#include "../module/end.hh"

#endif
