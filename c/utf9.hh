#ifndef utf9_h
#define utf9_h


#include <bit>
#include <cstdint>
#include <iostream>
#include <list>
#include <stdexcept>
#include <string>
#include <vector>

#include <byteswap.h>
#include <unistd.h>


namespace tau::utf9
{


// Input buffer of bytes to be parsed as bytecode
struct ibuf
{
  uint8_t const * xs;
  uint64_t        l;

  bool has  (int64_t i) const { return i >= 0 && i < l; }
  void check(int64_t i) const { if (!has(i)) throw std::length_error("OOBE"); }

  uint64_t len  (uint64_t i) const;
  uint64_t tlen (uint64_t i) const;
  uint64_t tvlen(uint64_t i) const;

  uint8_t  u8 (uint64_t i) const { return xs[i]; }
  uint16_t u16(uint64_t i) const
    {
      if constexpr (std::endian::native == std::endian::little)
                     return __bswap_16(*(uint16_t*)(xs + i));
      else if constexpr (std::endian::native == std::endian::little)
                          return *(uint16_t*)(xs + i);
    }

  uint32_t u32(uint64_t i) const
    {
      if constexpr (std::endian::native == std::endian::little)
                     return __bswap_32(*(uint32_t*)(xs + i));
      else if constexpr (std::endian::native == std::endian::little)
                          return *(uint32_t*)(xs + i);
    }

  uint64_t u64(uint64_t i) const
    {
      if constexpr (std::endian::native == std::endian::little)
                     return __bswap_64(*(uint64_t*)(xs + i));
      else if constexpr (std::endian::native == std::endian::little)
                          return *(uint64_t*)(xs + i);
    }
};


typedef uint64_t(*lfn)(ibuf const &, uint64_t);


namespace
{

#define lf(body) [](ibuf const &b, uint64_t i) -> uint64_t { return static_cast<uint64_t>(body); }

// Small constant lengths factored here to minimize
// instruction cache misses
lfn const l1  = lf(1);
lfn const l2  = lf(2);
lfn const l3  = lf(3);
lfn const l4  = lf(4);
lfn const l5  = lf(5);
lfn const l6  = lf(6);
lfn const l7  = lf(7);
lfn const l8  = lf(8);
lfn const l9  = lf(9);
lfn const l17 = lf(17);


// Value-length functions
lfn const fixutf8_lf   = lf(1 + (b.u8(i) - 0x20));
lfn const fixbytes_lf  = lf(1 + (b.u8(i) - 0x30));
lfn const fixtuple8_lf = lf(1 + b.u8(i + 1));
lfn const fixint_lf    = l1;

lfn const idx16_lf = lf(1 + 2 + b.u16(i + 1));
lfn const idx32_lf = lf(1 + 4 + b.u32(i + 1));
lfn const idx64_lf = lf(1 + 8 + b.u64(i + 1));

lfn const bogus_lf =
  [](ibuf const &b, uint64_t i) -> uint64_t
  { throw std::invalid_argument("bogus"); };


// Typecode value length functions
lfn const fixutf8_tvlf   = lf(b.u8(i) - 0x20);
lfn const fixbytes_tvlf  = lf(b.u8(i) - 0x30);
lfn const fixtuple8_tvlf = lf(b.u8(i + 1));


// Value-length decoders
lfn const lfns[256] =
{
  // 0x00 - 0x0f
  l2, l3, l5, l9,
  l2, l3, l5, l9,
  l5, l9, l9, l9,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  // 0x10 - 0x1f
  l1, l1, l1, l1,
  l1, l3, l5, l9,

  lf(2 + b.u8 (i + 1)),
  lf(3 + b.u16(i + 1)),
  lf(5 + b.u32(i + 1)),
  lf(9 + b.u64(i + 1)),
  lf(2 + b.u8 (i + 1)),
  lf(3 + b.u16(i + 1)),
  lf(5 + b.u32(i + 1)),
  lf(9 + b.u64(i + 1)),

  // 0x2N
  fixutf8_lf, fixutf8_lf, fixutf8_lf, fixutf8_lf,
  fixutf8_lf, fixutf8_lf, fixutf8_lf, fixutf8_lf,
  fixutf8_lf, fixutf8_lf, fixutf8_lf, fixutf8_lf,
  fixutf8_lf, fixutf8_lf, fixutf8_lf, fixutf8_lf,

  // 0x3N
  fixbytes_lf, fixbytes_lf, fixbytes_lf, fixbytes_lf,
  fixbytes_lf, fixbytes_lf, fixbytes_lf, fixbytes_lf,
  fixbytes_lf, fixbytes_lf, fixbytes_lf, fixbytes_lf,
  fixbytes_lf, fixbytes_lf, fixbytes_lf, fixbytes_lf,

  // 0x40-0x4f
  lf(3  + b.u8 (i + 1)),
  lf(5  + b.u16(i + 1)),
  lf(9  + b.u32(i + 1)),
  lf(17 + b.u64(i + 1)),
  lf(3  + b.u8 (i + 1)),
  lf(5  + b.u16(i + 1)),
  lf(9  + b.u32(i + 1)),
  lf(17 + b.u64(i + 1)),
  fixtuple8_lf, fixtuple8_lf, fixtuple8_lf, fixtuple8_lf,
  fixtuple8_lf, fixtuple8_lf, fixtuple8_lf, fixtuple8_lf,

  // 0x50-0x5f
  lf(1 + 2 + 1 + b.u16(i + 1)),
  lf(1 + 4 + 1 + b.u32(i + 1)),
  lf(1 + 8 + 1 + b.u64(i + 1)),
  l1,

  idx16_lf, idx16_lf, idx16_lf, idx16_lf,
  idx16_lf, idx16_lf, idx16_lf, idx16_lf,
  idx16_lf, idx16_lf, idx16_lf, idx16_lf,

  // 0x60-0x6f
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  idx32_lf, idx32_lf, idx32_lf, idx32_lf,
  idx32_lf, idx32_lf, idx32_lf, idx32_lf,
  idx32_lf, idx32_lf, idx32_lf, idx32_lf,

  // 0x70-0x7f
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  idx64_lf, idx64_lf, idx64_lf, idx64_lf,
  idx64_lf, idx64_lf, idx64_lf, idx64_lf,
  idx64_lf, idx64_lf, idx64_lf, idx64_lf,

  // 0x80-0xbf
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,

  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,

  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,

  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,

  // 0xc0-0xff
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,

  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,

  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,

  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
  fixint_lf, fixint_lf, fixint_lf, fixint_lf,
};

lfn const tlfns[256] =
{
  // 0x00-0x0f
  l1, l1, l1, l1,
  l1, l1, l1, l1,
  l1, l1, l1, l1,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  // 0x10-0x1f
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  l2, l3, l5, l9,
  l2, l3, l5, l9,

  // 0x20-0x2f
  l1, l1, l1, l1,
  l1, l1, l1, l1,
  l1, l1, l1, l1,
  l1, l1, l1, l1,

  // 0x30-0x3f
  l1, l1, l1, l1,
  l1, l1, l1, l1,
  l1, l1, l1, l1,
  l1, l1, l1, l1,

  // 0x40-0x4f
  l3, l5, l9, l17,

  lf(3  + b.tlen(i + 3)),
  lf(5  + b.tlen(i + 5)),
  lf(9  + b.tlen(i + 9)),
  lf(17 + b.tlen(i + 17)),

  l2, l2, l2, l2,
  l2, l2, l2, l2,

  // 0x50-0x5f
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  // 0x60-0x6f
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  // 0x70-0x7f
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  // 0x80-0xbf
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  // 0xc0-0xff
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
};

lfn const tvlfns[256] =
{
  // 0x00-0x0f
  l1, l2, l4, l8,
  l1, l2, l4, l8,
  l4, l8, l8, l8,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  // 0x10-0x1f
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  lf(b.u8 (i + 1)),
  lf(b.u16(i + 1)),
  lf(b.u32(i + 1)),
  lf(b.u64(i + 1)),
  lf(b.u8 (i + 1)),
  lf(b.u16(i + 1)),
  lf(b.u32(i + 1)),
  lf(b.u64(i + 1)),

  // 0x20-0x2f
  fixutf8_tvlf, fixutf8_tvlf, fixutf8_tvlf, fixutf8_tvlf,
  fixutf8_tvlf, fixutf8_tvlf, fixutf8_tvlf, fixutf8_tvlf,
  fixutf8_tvlf, fixutf8_tvlf, fixutf8_tvlf, fixutf8_tvlf,
  fixutf8_tvlf, fixutf8_tvlf, fixutf8_tvlf, fixutf8_tvlf,

  // 0x30-0x3f
  fixbytes_tvlf, fixbytes_tvlf, fixbytes_tvlf, fixbytes_tvlf,
  fixbytes_tvlf, fixbytes_tvlf, fixbytes_tvlf, fixbytes_tvlf,
  fixbytes_tvlf, fixbytes_tvlf, fixbytes_tvlf, fixbytes_tvlf,
  fixbytes_tvlf, fixbytes_tvlf, fixbytes_tvlf, fixbytes_tvlf,

  // 0x40-0x4f
  lf(b.u8 (i + 1)),
  lf(b.u16(i + 1)),
  lf(b.u32(i + 1)),
  lf(b.u64(i + 1)),
  lf(b.u8 (i + 1)),
  lf(b.u16(i + 1)),
  lf(b.u32(i + 1)),
  lf(b.u64(i + 1)),

  fixtuple8_tvlf, fixtuple8_tvlf, fixtuple8_tvlf, fixtuple8_tvlf,
  fixtuple8_tvlf, fixtuple8_tvlf, fixtuple8_tvlf, fixtuple8_tvlf,

  // 0x50-0x5f
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  // 0x60-0x6f
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  // 0x70-0x7f
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  // 0x80-0xbf
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  // 0xc0-0xff
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,
};


#undef lf

}


inline uint64_t ibuf::len  (uint64_t i) const { return lfns[xs[i]](*this, i); }
inline uint64_t ibuf::tlen (uint64_t i) const { return tlfns[xs[i]](*this, i); }
inline uint64_t ibuf::tvlen(uint64_t i) const { return tvlfns[xs[i]](*this, i); }


}

#endif  // ifndef utf9_h
