#ifndef utf9_record_h
#define utf9_record_h


#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include <unistd.h>


namespace tau {
namespace utf9 {

  struct val;
  struct bytecode
  {
    std::vector<uint8_t> const *const xs;
    uint64_t                    const i;

    // FIXME: check bounds once here when we construct a val
    val operator*() const { /* TODO */ }

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


  // FIXME: bytecode should be a delta?
  // FIXME: template this
  struct size_next { uint64_t size; bytecode next; };

  typedef bytecode(*next_fn)(bytecode);
  typedef size_next(*size_fn)(bytecode);


  // FIXME: use std::iterator for this

#define sf(size, next) [](bytecode const x) -> size_next { return {static_cast<uint64_t>(size), (next)}; }

  namespace
  {
    size_fn const fixbytes_sf  = sf(x.u8(0) - 0x20, x + 1);
    size_fn const fixstr_sf    = sf(x.u8(0) - 0x30, x + 1);
    size_fn const fixtuple8_sf = sf(x.u8(1), x + 2);
    size_fn const bogus_sf     = [](bytecode x) -> size_next
      { std::cerr << "sf[" << x.u8(0) << "]@" << x << std::endl;
        _exit(1);
        return {0, x}; };

    inline size_next atsize(bytecode const);
  }

  size_fn const sfs[256] =
  {
    // 0x00-0x0f
    sf(1, x + 1),
    sf(2, x + 1),
    sf(4, x + 1),
    sf(8, x + 1),
    sf(1, x + 1),
    sf(2, x + 1),
    sf(4, x + 1),
    sf(8, x + 1),

    sf(4, x + 1),
    sf(8, x + 1),
    sf(8, x + 1),
    sf(8, x + 1),
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

    sf(x.u8(1),  x + 2),
    sf(x.u16(1), x + 3),
    sf(x.u32(1), x + 5),
    sf(x.u64(1), x + 9),
    sf(x.u8(1),  x + 2),
    sf(x.u16(1), x + 3),
    sf(x.u32(1), x + 5),
    sf(x.u64(1), x + 9),

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
    sf(x.u8(1),  x + 3),
    sf(x.u16(1), x + 5),
    sf(x.u32(1), x + 9),
    sf(x.u64(1), x + 17),
    [](bytecode x) -> size_next { auto sn = atsize(x + 1); return { sn.next.u8(0)  * sn.size, sn.next + 1}; },
    [](bytecode x) -> size_next { auto sn = atsize(x + 1); return { sn.next.u16(0) * sn.size, sn.next + 2}; },
    [](bytecode x) -> size_next { auto sn = atsize(x + 1); return { sn.next.u32(0) * sn.size, sn.next + 4}; },
    [](bytecode x) -> size_next { auto sn = atsize(x + 1); return { sn.next.u64(0) * sn.size, sn.next + 8}; },

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


#define nf(body) [](bytecode const x) { return (body); }

  namespace
  {
    next_fn const fixbytes_nf  = nf(x + 1 + (x.u8(0) - 0x20));
    next_fn const fixstr_nf    = nf(x + 1 + (x.u8(0) - 0x30));
    next_fn const fixtuple8_nf = nf(x + 1 + x.u8(1));
    next_fn const fixint_nf    = nf(x + 1);
    next_fn const bogus_nf     = [](bytecode x)
      { std::cerr << "nf[" << x.u8(0) << "]@" << x << std::endl;
        _exit(1);
        return x; };
  }

  next_fn const nfs[256] =
  {
    // 0x00 - 0x0f
    nf(x + 2),
    nf(x + 3),
    nf(x + 5),
    nf(x + 9),
    nf(x + 2),
    nf(x + 3),
    nf(x + 5),
    nf(x + 9),

    nf(x + 5),  // float32
    nf(x + 9),
    nf(x + 9),
    nf(x + 9),  // pidfd
    bogus_nf,
    bogus_nf,
    bogus_nf,
    bogus_nf,

    // 0x10 - 0x1f
    nf(x + 1),
    nf(x + 1),
    nf(x + 1),
    nf(x + 1),
    nf(x + 1),
    nf(x + 3),
    nf(x + 5),
    nf(x + 9),

    nf(x + 2 + x.u8(1)),       // 0x18: utf8 uint8
    nf(x + 3 + x.u16(1)),
    nf(x + 5 + x.u32(1)),
    nf(x + 9 + x.u64(1)),
    nf(x + 2 + x.u8(1)),
    nf(x + 3 + x.u16(1)),
    nf(x + 5 + x.u32(1)),
    nf(x + 9 + x.u64(1)),

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
    nf(x + 3  + x.u8(1)),
    nf(x + 5  + x.u16(1)),
    nf(x + 9  + x.u32(1)),
    nf(x + 17 + x.u64(1)),
    [](bytecode x) { auto sn = atsize(x + 1); return sn.next + 1 + sn.next.u8(0)  * sn.size; },
    [](bytecode x) { auto sn = atsize(x + 1); return sn.next + 2 + sn.next.u16(0) * sn.size; },
    [](bytecode x) { auto sn = atsize(x + 1); return sn.next + 4 + sn.next.u32(0) * sn.size; },
    [](bytecode x) { auto sn = atsize(x + 1); return sn.next + 8 + sn.next.u64(0) * sn.size; },

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
    uint64_t size() const { return ti ? 0                               : nfs[*p](p) - p; }
    uint64_t len()  const { }  // TODO: fn table

    // TODO: fn tables for these
    operator int()    const { return 0; /* TODO */ }
    operator double() const { return 0; /* TODO */ }

    // FIXME: we should have generalized "data is N items of X type and starts here"
    // to provide this, not manual switch()
    operator std::vector<val>() const
      {
        switch (type())
        {
        case TUPLE:
        {
          uint64_t n = 0;
          bytecode c;
        }

        case ARRAY:
        {
          auto     sn = atsize(p + 1);
          uint64_t n  = 0;
          switch (*p)
          {
          case 0x44: n =       *sn.next;  sn.next++;    break;
          case 0x45: n = ru16be(sn.next); sn.next += 2; break;
          case 0x46: n = ru32be(sn.next); sn.next += 4; break;
          case 0x47: n = ru64be(sn.next); sn.next += 8; break;
          }

          std::vector<val> r;
          r.reserve(n);
          for (int i = 0; i < n; ++i)
          {
            r.push_back(val{p, sn.next});
            sn.next = nfs[*sn.next](sn.next);
          }
          return std::move(r);
        }

        case IDX:
        {
          std::cerr << "unimplemented: IDX -> vector" << std::endl;
          _exit(1);
        }

        default:
        {
          std::cerr << "sv<" << *p << ">@" << p << std::endl;
          _exit(1);
        }
        }
      }

    cursor operator[](val v) const
      {
        switch (type())
        {
        default:
          return cursor{0};
        }
      }

    cursor operator[](uint64_t i) const
      {
        switch (type())
        {
        default:
          return cursor{0};
        }

      }

  };

}
}

#endif
