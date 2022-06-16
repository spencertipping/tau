#ifndef utf9_record_h
#define utf9_record_h


#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include <unistd.h>


namespace tau::utf9
{


// TODO: this should be a buffer class, and we should bounds-check once per value
// being created _from here_, not internally to other values
struct bytecode
{
  std::vector<uint8_t> const *const xs;
  uint64_t                    const i;

  bytecode operator+(int offset)   const { return {xs, i + offset}; }
  int      operator-(bytecode rhs) const { return i - rhs.i; }

  // NOTE: functions below execute a single bounds-check
  uint8_t  u8 (int offset) const { return xs->at(i + offset); }
  uint16_t u16(int offset) const { return (uint16_t) ((*xs)[i + offset]) << 8
                                        | (uint16_t) xs->at(i + offset + 1); }

  uint32_t u32(int offset) const { return (uint32_t) ((*xs)[i + offset    ]) << 24
                                        | (uint32_t) ((*xs)[i + offset + 1]) << 16
                                        | (uint32_t) ((*xs)[i + offset + 2]) << 8
                                        | (uint32_t) xs->at(i + offset + 3); }

  uint64_t u64(int offset) const { return (uint64_t) ((*xs)[i + offset   ]) << 56
                                        | (uint64_t) ((*xs)[i + offset + 1]) << 48
                                        | (uint64_t) ((*xs)[i + offset + 2]) << 40
                                        | (uint64_t) ((*xs)[i + offset + 3]) << 32
                                        | (uint64_t) ((*xs)[i + offset + 4]) << 24
                                        | (uint64_t) ((*xs)[i + offset + 5]) << 16
                                        | (uint64_t) ((*xs)[i + offset + 6]) << 8
                                        | (uint64_t) xs->at(i + offset + 7); }
};

std::ostream &operator<<(std::ostream &s, bytecode const &b)
{
  s << "TODO: <<bytecode" << std::endl;
  return s;
}


// FIXME: template this
struct size_next { uint64_t size; uint64_t next; };

typedef uint64_t(*next_fn)(bytecode);
typedef size_next(*size_fn)(bytecode);


#define sf(size, next)                          \
  [](bytecode const x) -> size_next             \
  { return {static_cast<uint64_t>(size),        \
            static_cast<uint64_t>(next)}; }

namespace
{
  size_fn const fixbytes_sf  = sf(x.u8(0) - 0x20, 1);
  size_fn const fixstr_sf    = sf(x.u8(0) - 0x30, 1);
  size_fn const fixtuple8_sf = sf(x.u8(1), 2);
  size_fn const bogus_sf     = [](bytecode x) -> size_next
    { std::cerr << "sf[" << x.u8(0) << "]@" << x << std::endl;
      _exit(1);
      return {0, 0}; };

  inline size_next atsize(bytecode const);
}

// TODO: template this and nfs[] (and rename both) so that we can use checked+unchecked
// with the same .u8() helper syntax
size_fn const sfs[256] =
{
  // 0x00-0x0f
  sf(1, 1),
  sf(2, 1),
  sf(4, 1),
  sf(8, 1),
  sf(1, 1),
  sf(2, 1),
  sf(4, 1),
  sf(8, 1),

  sf(4, 1),
  sf(8, 1),
  sf(8, 1),
  sf(8, 1),
  bogus_sf,
  bogus_sf,
  bogus_sf,
  bogus_sf,

  // 0x10-0x1f
  bogus_sf,
  bogus_sf,
  bogus_sf,
  bogus_sf,
  bogus_sf,
  bogus_sf,
  bogus_sf,
  bogus_sf,

  sf(x.u8(1),  2),
  sf(x.u16(1), 3),
  sf(x.u32(1), 5),
  sf(x.u64(1), 9),
  sf(x.u8(1),  2),
  sf(x.u16(1), 3),
  sf(x.u32(1), 5),
  sf(x.u64(1), 9),

  // 0x20-0x2f
  fixbytes_sf, fixbytes_sf, fixbytes_sf, fixbytes_sf,
  fixbytes_sf, fixbytes_sf, fixbytes_sf, fixbytes_sf,
  fixbytes_sf, fixbytes_sf, fixbytes_sf, fixbytes_sf,
  fixbytes_sf, fixbytes_sf, fixbytes_sf, fixbytes_sf,

  // 0x30-0x3f
  fixstr_sf, fixstr_sf, fixstr_sf, fixstr_sf,
  fixstr_sf, fixstr_sf, fixstr_sf, fixstr_sf,
  fixstr_sf, fixstr_sf, fixstr_sf, fixstr_sf,
  fixstr_sf, fixstr_sf, fixstr_sf, fixstr_sf,

  // 0x40-0x4f
  sf(x.u8(1),  3),
  sf(x.u16(1), 5),
  sf(x.u32(1), 9),
  sf(x.u64(1), 17),
  sf(x.u8(1),  atsize(x + 3).next),
  sf(x.u8(1),  atsize(x + 5).next),
  sf(x.u8(1),  atsize(x + 9).next),
  sf(x.u8(1),  atsize(x + 17).next),

  fixtuple8_sf, fixtuple8_sf, fixtuple8_sf, fixtuple8_sf,
  fixtuple8_sf, fixtuple8_sf, fixtuple8_sf, fixtuple8_sf,

  // 0x50-0x5f
  bogus_sf, bogus_sf, bogus_sf, bogus_sf,
  bogus_sf, bogus_sf, bogus_sf, bogus_sf,
  bogus_sf, bogus_sf, bogus_sf, bogus_sf,
  bogus_sf, bogus_sf, bogus_sf, bogus_sf,

  // 0x60-0x6f
  bogus_sf, bogus_sf, bogus_sf, bogus_sf,
  bogus_sf, bogus_sf, bogus_sf, bogus_sf,
  bogus_sf, bogus_sf, bogus_sf, bogus_sf,
  bogus_sf, bogus_sf, bogus_sf, bogus_sf,

  // 0x70-0x7f
  bogus_sf, bogus_sf, bogus_sf, bogus_sf,
  bogus_sf, bogus_sf, bogus_sf, bogus_sf,
  bogus_sf, bogus_sf, bogus_sf, bogus_sf,
  bogus_sf, bogus_sf, bogus_sf, bogus_sf,

  // 0x80-0xbf
  bogus_sf, bogus_sf, bogus_sf, bogus_sf,
  bogus_sf, bogus_sf, bogus_sf, bogus_sf,
  bogus_sf, bogus_sf, bogus_sf, bogus_sf,
  bogus_sf, bogus_sf, bogus_sf, bogus_sf,

  bogus_sf, bogus_sf, bogus_sf, bogus_sf,
  bogus_sf, bogus_sf, bogus_sf, bogus_sf,
  bogus_sf, bogus_sf, bogus_sf, bogus_sf,
  bogus_sf, bogus_sf, bogus_sf, bogus_sf,

  bogus_sf, bogus_sf, bogus_sf, bogus_sf,
  bogus_sf, bogus_sf, bogus_sf, bogus_sf,
  bogus_sf, bogus_sf, bogus_sf, bogus_sf,
  bogus_sf, bogus_sf, bogus_sf, bogus_sf,

  bogus_sf, bogus_sf, bogus_sf, bogus_sf,
  bogus_sf, bogus_sf, bogus_sf, bogus_sf,
  bogus_sf, bogus_sf, bogus_sf, bogus_sf,
  bogus_sf, bogus_sf, bogus_sf, bogus_sf,

  // 0xc0-0xff
  bogus_sf, bogus_sf, bogus_sf, bogus_sf,
  bogus_sf, bogus_sf, bogus_sf, bogus_sf,
  bogus_sf, bogus_sf, bogus_sf, bogus_sf,
  bogus_sf, bogus_sf, bogus_sf, bogus_sf,

  bogus_sf, bogus_sf, bogus_sf, bogus_sf,
  bogus_sf, bogus_sf, bogus_sf, bogus_sf,
  bogus_sf, bogus_sf, bogus_sf, bogus_sf,
  bogus_sf, bogus_sf, bogus_sf, bogus_sf,

  bogus_sf, bogus_sf, bogus_sf, bogus_sf,
  bogus_sf, bogus_sf, bogus_sf, bogus_sf,
  bogus_sf, bogus_sf, bogus_sf, bogus_sf,
  bogus_sf, bogus_sf, bogus_sf, bogus_sf,

  bogus_sf, bogus_sf, bogus_sf, bogus_sf,
  bogus_sf, bogus_sf, bogus_sf, bogus_sf,
  bogus_sf, bogus_sf, bogus_sf, bogus_sf,
  bogus_sf, bogus_sf, bogus_sf, bogus_sf,
};

#undef sf

namespace
{
  inline size_next atsize(bytecode const b) { return sfs[b.u8(0)](b); }
}


#define nf(body) [](bytecode const x) -> uint64_t { return (body); }

namespace
{
  next_fn const fixbytes_nf  = nf(1 + (x.u8(0) - 0x20));
  next_fn const fixstr_nf    = nf(1 + (x.u8(0) - 0x30));
  next_fn const fixtuple8_nf = nf(1 + x.u8(1));
  next_fn const fixint_nf    = nf(1);
  next_fn const bogus_nf     = [](bytecode x) -> uint64_t
    { std::cerr << "nf[" << x.u8(0) << "]@" << x << std::endl;
      _exit(1);
      return 0; };
}

// FIXME: these should return sizeof, not next
next_fn const nfs[256] =
{
  // 0x00 - 0x0f
  nf(2),
  nf(3),
  nf(5),
  nf(9),
  nf(2),
  nf(3),
  nf(5),
  nf(9),

  nf(5),  // float32
  nf(9),
  nf(9),
  nf(9),  // pidfd
  bogus_nf,
  bogus_nf,
  bogus_nf,
  bogus_nf,

  // 0x10 - 0x1f
  nf(1),
  nf(1),
  nf(1),
  nf(1),
  nf(1),
  nf(3),
  nf(5),
  nf(9),

  nf(2 + x.u8(1)),       // 0x18: utf8 uint8
  nf(3 + x.u16(1)),
  nf(5 + x.u32(1)),
  nf(9 + x.u64(1)),
  nf(2 + x.u8(1)),
  nf(3 + x.u16(1)),
  nf(5 + x.u32(1)),
  nf(9 + x.u64(1)),

  // 0x2N
  fixbytes_nf, fixbytes_nf, fixbytes_nf, fixbytes_nf,
  fixbytes_nf, fixbytes_nf, fixbytes_nf, fixbytes_nf,
  fixbytes_nf, fixbytes_nf, fixbytes_nf, fixbytes_nf,
  fixbytes_nf, fixbytes_nf, fixbytes_nf, fixbytes_nf,

  // 0x3N
  fixstr_nf, fixstr_nf, fixstr_nf, fixstr_nf,
  fixstr_nf, fixstr_nf, fixstr_nf, fixstr_nf,
  fixstr_nf, fixstr_nf, fixstr_nf, fixstr_nf,
  fixstr_nf, fixstr_nf, fixstr_nf, fixstr_nf,

  // 0x40-0x4f
  nf(3  + x.u8(1)),
  nf(5  + x.u16(1)),
  nf(9  + x.u32(1)),
  nf(17 + x.u64(1)),
  nf(3  + x.u8(1)),
  nf(5  + x.u16(1)),
  nf(9  + x.u32(1)),
  nf(17 + x.u64(1)),
  fixtuple8_nf, fixtuple8_nf, fixtuple8_nf, fixtuple8_nf,
  fixtuple8_nf, fixtuple8_nf, fixtuple8_nf, fixtuple8_nf,

  // 0x50-0x5f
  bogus_nf, bogus_nf, bogus_nf, bogus_nf,
  bogus_nf, bogus_nf, bogus_nf, bogus_nf,
  bogus_nf, bogus_nf, bogus_nf, bogus_nf,
  bogus_nf, bogus_nf, bogus_nf, bogus_nf,

  // 0x60-0x6f
  bogus_nf, bogus_nf, bogus_nf, bogus_nf,
  bogus_nf, bogus_nf, bogus_nf, bogus_nf,
  bogus_nf, bogus_nf, bogus_nf, bogus_nf,
  bogus_nf, bogus_nf, bogus_nf, bogus_nf,

  // 0x70-0x7f
  bogus_nf, bogus_nf, bogus_nf, bogus_nf,
  bogus_nf, bogus_nf, bogus_nf, bogus_nf,
  bogus_nf, bogus_nf, bogus_nf, bogus_nf,
  bogus_nf, bogus_nf, bogus_nf, bogus_nf,

  // 0x80-0xbf
  fixint_nf, fixint_nf, fixint_nf, fixint_nf,
  fixint_nf, fixint_nf, fixint_nf, fixint_nf,
  fixint_nf, fixint_nf, fixint_nf, fixint_nf,
  fixint_nf, fixint_nf, fixint_nf, fixint_nf,

  fixint_nf, fixint_nf, fixint_nf, fixint_nf,
  fixint_nf, fixint_nf, fixint_nf, fixint_nf,
  fixint_nf, fixint_nf, fixint_nf, fixint_nf,
  fixint_nf, fixint_nf, fixint_nf, fixint_nf,

  fixint_nf, fixint_nf, fixint_nf, fixint_nf,
  fixint_nf, fixint_nf, fixint_nf, fixint_nf,
  fixint_nf, fixint_nf, fixint_nf, fixint_nf,
  fixint_nf, fixint_nf, fixint_nf, fixint_nf,

  fixint_nf, fixint_nf, fixint_nf, fixint_nf,
  fixint_nf, fixint_nf, fixint_nf, fixint_nf,
  fixint_nf, fixint_nf, fixint_nf, fixint_nf,
  fixint_nf, fixint_nf, fixint_nf, fixint_nf,

  // 0xc0-0xff
  fixint_nf, fixint_nf, fixint_nf, fixint_nf,
  fixint_nf, fixint_nf, fixint_nf, fixint_nf,
  fixint_nf, fixint_nf, fixint_nf, fixint_nf,
  fixint_nf, fixint_nf, fixint_nf, fixint_nf,

  fixint_nf, fixint_nf, fixint_nf, fixint_nf,
  fixint_nf, fixint_nf, fixint_nf, fixint_nf,
  fixint_nf, fixint_nf, fixint_nf, fixint_nf,
  fixint_nf, fixint_nf, fixint_nf, fixint_nf,

  fixint_nf, fixint_nf, fixint_nf, fixint_nf,
  fixint_nf, fixint_nf, fixint_nf, fixint_nf,
  fixint_nf, fixint_nf, fixint_nf, fixint_nf,
  fixint_nf, fixint_nf, fixint_nf, fixint_nf,

  fixint_nf, fixint_nf, fixint_nf, fixint_nf,
  fixint_nf, fixint_nf, fixint_nf, fixint_nf,
  fixint_nf, fixint_nf, fixint_nf, fixint_nf,
  fixint_nf, fixint_nf, fixint_nf, fixint_nf,
};

#undef nf

enum val_type
{
  NONE,
  INT,
  FLOAT32,
  FLOAT64,
  SYMBOL,
  PIDFD,
  GREEK,

  UTF8,
  BYTES,
  TUPLE,
  ARRAY,
  IDX
};

uint8_t const bts[256] =
{
  // 0x00-0x0f
  INT, INT, INT, INT,
  INT, INT, INT, INT,
  FLOAT32, FLOAT64, SYMBOL, PIDFD,
  NONE, NONE, NONE, NONE,

  // 0x10-0x1f
  GREEK, GREEK, GREEK, GREEK,
  GREEK, GREEK, GREEK, GREEK,
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
  NONE, NONE, NONE, NONE,
  NONE, NONE, NONE, NONE,
  NONE, NONE, NONE, NONE,
  NONE, NONE, NONE, NONE,

  // 0x60-0x6f
  NONE, NONE, NONE, NONE,
  NONE, NONE, NONE, NONE,
  NONE, NONE, NONE, NONE,
  NONE, NONE, NONE, NONE,

  // 0x70-0x7f
  NONE, NONE, NONE, NONE,
  NONE, NONE, NONE, NONE,
  NONE, NONE, NONE, NONE,
  NONE, NONE, NONE, NONE,

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

enum type_index : uint64_t
{
  BYTECODE = 0,
  INTEGER  = 1,
  DOUBLE   = 2,
  FLOAT    = 3,
};


struct val
{
  union
  {
    uint8_t const * const tp;
    uint64_t              ti;
  };

  union
  {
    uint8_t const * const p;
    uint64_t              i;
    double                d;
    float                 f;
  };

  val_type type() const { return ti ? static_cast<val_type>(bts[*tp]) : static_cast<val_type>(bts[*p]); }
  uint64_t size() const { return ti ? 0                               : nfs[*p](p); }
  uint64_t len()  const { }  // TODO: fn table

  // TODO: fn tables for these
  operator int()    const { return 0; /* TODO */ }
  operator double() const { return 0; /* TODO */ }
};


}

#endif
