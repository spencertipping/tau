#ifndef utf9_record_h
#define utf9_record_h


#include <cstdint>
#include <iostream>
#include <string>

#include <byteswap.h>
#include <unistd.h>


namespace tau
{
  namespace utf9
  {
    typedef uint8_t *bytecode;

    struct size_next { uint64_t size; bytecode next; };

    typedef bytecode(*next_fn)(bytecode);
    typedef size_next(*size_fn)(bytecode);


    enum bytecodes
    {
      UTF9_UINT8    = 0x00,
      UTF9_UINT16   = 0x01,
      UTF9_UINT32   = 0x02,
      UTF9_UINT64   = 0x03,
      UTF9_INT8     = 0x04,
      UTF9_INT16    = 0x05,
      UTF9_INT32    = 0x06,
      UTF9_INT64    = 0x07,
      UTF9_FLOAT32  = 0x08,
      UTF9_FLOAT64  = 0x09,
      UTF9_SYMBOL   = 0x0a,
      UTF9_PIDFD    = 0x0b,
      UTF9_ALPHA    = 0x10,
      UTF9_OMEGA    = 0x11,
      UTF9_IOTA     = 0x12,
      UTF9_KAPPA    = 0x13,
      UTF9_TAU      = 0x14,
      UTF9_UTF8_8   = 0x18,
      UTF9_UTF8_16  = 0x19,
      UTF9_UTF8_32  = 0x1a,
      UTF9_UTF8_64  = 0x1b,
      UTF9_BYTES_8  = 0x1c,
      UTF9_BYTES_16 = 0x1d,
      UTF9_BYTES_32 = 0x1e,
      UTF9_BYTES_64 = 0x1f,
      UTF9_TUPLE_8  = 0x40,
      UTF9_TUPLE_16 = 0x41,
      UTF9_TUPLE_32 = 0x42,
      UTF9_TUPLE_64 = 0x43,
      UTF9_ARRAY_8  = 0x44,
      UTF9_ARRAY_16 = 0x45,
      UTF9_ARRAY_32 = 0x46,
      UTF9_ARRAY_64 = 0x47,
      UTF9_CIDX_16  = 0x49,
      UTF9_CIDX_32  = 0x4a,
      UTF9_CIDX_64  = 0x4b,
      UTF9_PIDX_8   = 0x4c,
      UTF9_PIDX_16  = 0x4d,
      UTF9_PIDX_32  = 0x4e,
      UTF9_PIDX_64  = 0x4f,
    };


    struct byte_bitfield
    {
      uint16_t bits[8] = {0};

      byte_bitfield &operator|=(uint8_t const x)
        { bits[x >> 4] |= 1 << (x & 15);
          return *this; }

      bool operator&(uint8_t const x) { return bits[x >> 4] & 1 << (x & 15); }
    };


#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    static uint16_t ru16be(bytecode b) { return __bswap_16(*reinterpret_cast<uint16_t*>(b)); }
    static uint32_t ru32be(bytecode b) { return __bswap_32(*reinterpret_cast<uint32_t*>(b)); }
    static uint64_t ru64be(bytecode b) { return __bswap_64(*reinterpret_cast<uint64_t*>(b)); }
#elif __BYTE_ORDER__ == ___ORDER_BIG_ENDIAN__
    static uint16_t ru16be(bytecode b) { return *reinterpret_cast<uint16_t*>(b); }
    static uint32_t ru32be(bytecode b) { return *reinterpret_cast<uint32_t*>(b); }
    static uint64_t ru64be(bytecode b) { return *reinterpret_cast<uint64_t*>(b); }
#endif


#define sf(size, next) [](bytecode const x) -> size_next { return {static_cast<uint64_t>(size), (next)}; }

    namespace
    {
      size_fn const fixbytes_sf   = sf(*x - 0x20, x + 1);
      size_fn const fixstr_sf     = sf(*x - 0x30, x + 1);
      size_fn const fixtuple8_sf  = sf(     *(x + 1), x + 2);
      size_fn const fixtuple16_sf = sf(ru16be(x + 1), x + 3);
      size_fn const bogus_sf      = [](bytecode x) -> size_next
        { std::cerr << "sf[" << *x << "]@" << x << std::endl;
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

      sf(     *(x + 1), x + 2),
      sf(ru16be(x + 1), x + 3),
      sf(ru32be(x + 1), x + 5),
      sf(ru64be(x + 1), x + 9),
      sf(     *(x + 1), x + 2),
      sf(ru16be(x + 1), x + 3),
      sf(ru32be(x + 1), x + 5),
      sf(ru64be(x + 1), x + 9),

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
      sf(     *(x + 1), x + 3),
      sf(ru16be(x + 1), x + 5),
      sf(ru32be(x + 1), x + 9),
      sf(ru64be(x + 1), x + 17),
      [](bytecode x) -> size_next { auto sn = atsize(x + 1); return {      *sn.next  * sn.size, sn.next + 1}; },
      [](bytecode x) -> size_next { auto sn = atsize(x + 1); return {ru16be(sn.next) * sn.size, sn.next + 2}; },
      [](bytecode x) -> size_next { auto sn = atsize(x + 1); return {ru32be(sn.next) * sn.size, sn.next + 4}; },
      [](bytecode x) -> size_next { auto sn = atsize(x + 1); return {ru64be(sn.next) * sn.size, sn.next + 8}; },

      bogus_sf, bogus_sf, bogus_sf, bogus_sf,  // indexes aren't valid array elements
      bogus_sf, bogus_sf, bogus_sf, bogus_sf,

      // 0x50-0x5f
      fixtuple8_sf, fixtuple8_sf, fixtuple8_sf, fixtuple8_sf,
      fixtuple8_sf, fixtuple8_sf, fixtuple8_sf, fixtuple8_sf,
      fixtuple8_sf, fixtuple8_sf, fixtuple8_sf, fixtuple8_sf,
      fixtuple8_sf, fixtuple8_sf, fixtuple8_sf, fixtuple8_sf,

      // 0x60-0x6f
      fixtuple16_sf, fixtuple16_sf, fixtuple16_sf, fixtuple16_sf,
      fixtuple16_sf, fixtuple16_sf, fixtuple16_sf, fixtuple16_sf,
      fixtuple16_sf, fixtuple16_sf, fixtuple16_sf, fixtuple16_sf,
      fixtuple16_sf, fixtuple16_sf, fixtuple16_sf, fixtuple16_sf,

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
      inline size_next atsize(bytecode const b) { return sfs[*b](b); }
    }


#define nf(body) [](bytecode const x) { return (body); }

    namespace
    {
      next_fn const fixbytes_nf   = nf(x + 1 + (*x - 0x20));
      next_fn const fixstr_nf     = nf(x + 1 + (*x - 0x30));
      next_fn const fixtuple8_nf  = nf(x + 1 + *(x + 1));
      next_fn const fixtuple16_nf = nf(x + 2 + ru16be(x + 1));
      next_fn const fixint_nf     = nf(x + 1);
      next_fn const bogus_nf      = [](bytecode x)
        { std::cerr << "nf[" << *x << "]@" << x << std::endl;
          _exit(1);
          return x; };

      inline bytecode next(bytecode);
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

      nf(x + 2 + *(x + 1)),       // 0x18: utf8 uint8
      nf(x + 3 + ru16be(x + 1)),
      nf(x + 5 + ru32be(x + 1)),
      nf(x + 9 + ru64be(x + 1)),
      nf(x + 2 + *(x + 1)),
      nf(x + 3 + ru16be(x + 1)),
      nf(x + 5 + ru32be(x + 1)),
      nf(x + 9 + ru64be(x + 1)),

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
      nf(x + 3  + *(x + 1)),
      nf(x + 5  + ru16be(x + 1)),
      nf(x + 9  + ru32be(x + 1)),
      nf(x + 17 + ru64be(x + 1)),
      [](bytecode x) { auto sn = atsize(x + 1); return sn.next + 1 +       *sn.next  * sn.size; },
      [](bytecode x) { auto sn = atsize(x + 1); return sn.next + 2 + ru16be(sn.next) * sn.size; },
      [](bytecode x) { auto sn = atsize(x + 1); return sn.next + 4 + ru32be(sn.next) * sn.size; },
      [](bytecode x) { auto sn = atsize(x + 1); return sn.next + 8 + ru64be(sn.next) * sn.size; },

      // FIXME: indexes
      bogus_nf, bogus_nf, bogus_nf, bogus_nf,
      bogus_nf, bogus_nf, bogus_nf, bogus_nf,

      // 0x50-0x5f
      fixtuple8_nf, fixtuple8_nf, fixtuple8_nf, fixtuple8_nf,
      fixtuple8_nf, fixtuple8_nf, fixtuple8_nf, fixtuple8_nf,
      fixtuple8_nf, fixtuple8_nf, fixtuple8_nf, fixtuple8_nf,
      fixtuple8_nf, fixtuple8_nf, fixtuple8_nf, fixtuple8_nf,

      // 0x60-0x6f
      fixtuple16_nf, fixtuple16_nf, fixtuple16_nf, fixtuple16_nf,
      fixtuple16_nf, fixtuple16_nf, fixtuple16_nf, fixtuple16_nf,
      fixtuple16_nf, fixtuple16_nf, fixtuple16_nf, fixtuple16_nf,
      fixtuple16_nf, fixtuple16_nf, fixtuple16_nf, fixtuple16_nf,

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

    namespace
    {
      inline bytecode next(bytecode const x) { return nfs[*x](x); }
    }

#undef nf


    struct cursor
    {
      bytecode p;
    };


    struct val
    {
      bytecode p;
    };

  }
}

#endif
