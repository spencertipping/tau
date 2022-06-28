#ifndef utf9_h
#define utf9_h


#include <algorithm>
#include <bit>
#include <cstdint>
#include <exception>
#include <functional>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <byteswap.h>

#include "xxHash/xxh3.h"


namespace tau::utf9
{

#define let auto const

#define LE (std::endian::native == std::endian::little)
#define BE (std::endian::native == std::endian::big)

static_assert(LE || BE, "unsupported endianness");

static_assert(sizeof(double) == sizeof(uint64_t));
static_assert(sizeof(float)  == sizeof(uint32_t));
static_assert(sizeof(void*)  == sizeof(uint64_t));
static_assert(sizeof(char)   == sizeof(uint8_t));


enum error : uint8_t
{
  ALIGNMENT_ERROR,
  BOUNDS_ERROR,
  IBUF_REQUIRED_ERROR,
  INVALID_TYPE_ERROR,
  COMPARE_ACROSS_TYPE_ERROR,
  COMPARE_ACROSS_ARRAY_TYPE_ERROR,
  NOT_COMPARABLE_ERROR,
  NOT_HASHABLE_ERROR,

  SIGNED_COERCION_ERROR,

  BOGUS_PF_ERROR,
  BOGUS_LF_ERROR,

  INVALID_BYTECODE_ERROR,
  INVALID_TYPECODE_ERROR,

  INTERNAL_ERROR,
};


namespace  // Consistent-endianness (i.e. big-endian in memory)
{

inline uint64_t ce(uint64_t x) { return LE ? __bswap_64(x) : x; }
inline uint32_t ce(uint32_t x) { return LE ? __bswap_32(x) : x; }
inline uint16_t ce(uint16_t x) { return LE ? __bswap_16(x) : x; }

inline int64_t  ce(int64_t x)  { return LE ? __bswap_64(x) : x; }
inline int32_t  ce(int32_t x)  { return LE ? __bswap_32(x) : x; }
inline int16_t  ce(int16_t x)  { return LE ? __bswap_16(x) : x; }

inline double ce(double const x)
{
  if (BE) return x;
  let y = bswap_64(*reinterpret_cast<uint64_t const*>(&x));
  return *reinterpret_cast<double const*>(&y);
}

inline float ce(float const x)
{
  if (BE) return x;
  let y = bswap_32(*reinterpret_cast<uint32_t const*>(&x));
  return *reinterpret_cast<float const*>(&y);
}

}


inline uint64_t xxh(void const * x, size_t l, uint64_t s)
{ return XXH64(x, l, s); }

inline uint64_t xxc(uint64_t a, uint64_t b)
{ a = ce(a); return XXH64(&a, sizeof(a), b); }


namespace  // Dispatch table compression
{

typedef int16_t tpacked;

void pack_table(uint64_t const *       t,
                uint64_t const         base,
                tpacked        * const p,
                int              const n)
{
  for (int i = 0; i < n; ++i)
  { p[i] = static_cast<tpacked>(t[i] - base);
    if (p[i] + base != t[i]) throw std::length_error("pack_table overflow"); }
}

}


struct ibuf
{
  uint8_t const * const xs;
  uint64_t        const l;


  ibuf(uint8_t const *xs_, uint64_t l_) : xs(xs_), l(l_) {}


  bool has  (uint64_t i) const { return i < l; }
  void check(uint64_t i) const { if (!has(i)) throw BOUNDS_ERROR; }

  uint64_t len  (uint64_t i) const;
  uint64_t tlen (uint64_t i) const;
  uint64_t tvlen(uint64_t i) const;


  uint8_t const *operator+(uint64_t i) const { return xs + i; }


  uint8_t  u8 (uint64_t i) const { return xs[i]; }
  uint16_t u16(uint64_t i) const { return ce(*reinterpret_cast<uint16_t const*>(xs + i)); }
  uint32_t u32(uint64_t i) const { return ce(*reinterpret_cast<uint32_t const*>(xs + i)); }
  uint64_t u64(uint64_t i) const { return ce(*reinterpret_cast<uint64_t const*>(xs + i)); }

  int8_t   i8 (uint64_t i) const { return reinterpret_cast<int8_t const*>(xs)[i]; }
  int16_t  i16(uint64_t i) const { return ce(*reinterpret_cast<int16_t const*>(xs + i)); }
  int32_t  i32(uint64_t i) const { return ce(*reinterpret_cast<int32_t const*>(xs + i)); }
  int64_t  i64(uint64_t i) const { return ce(*reinterpret_cast<int64_t const*>(xs + i)); }

  uint8_t  ci8 (uint64_t i) const { check(i);     return i8(i);  }
  uint16_t ci16(uint64_t i) const { check(i + 1); return i16(i); }
  uint32_t ci32(uint64_t i) const { check(i + 3); return i32(i); }
  uint64_t ci64(uint64_t i) const { check(i + 7); return i64(i); }

  uint8_t  cu8 (uint64_t i) const { check(i);     return u8(i);  }
  uint16_t cu16(uint64_t i) const { check(i + 1); return u16(i); }
  uint32_t cu32(uint64_t i) const { check(i + 3); return u32(i); }
  uint64_t cu64(uint64_t i) const { check(i + 7); return u64(i); }

  float  f32(uint64_t i) const { let x = u32(i); return ce(*reinterpret_cast<float const*>(&x)); }
  double f64(uint64_t i) const { let x = u64(i); return ce(*reinterpret_cast<double const*>(&x)); }
};


namespace  // Length traversal dispatch
{

typedef uint64_t(*lfn)(ibuf const &, uint64_t);
#define lf(body) [](ibuf const &b, uint64_t i) -> uint64_t { return static_cast<uint64_t>(body); }


let l1  = lf(1);
let l2  = lf(2);
let l3  = lf(3);
let l4  = lf(4);
let l5  = lf(5);
let l6  = lf(6);
let l7  = lf(7);
let l8  = lf(8);
let l9  = lf(9);
let l17 = lf(17);


// Value-length functions
let str8_lf   = lf(2 + b.u8 (i + 1));
let str16_lf  = lf(3 + b.u16(i + 1));
let str32_lf  = lf(5 + b.u32(i + 1));
let str64_lf  = lf(9 + b.u64(i + 1));

let list8_lf  = lf(3  + b.u8 (i + 1));
let list16_lf = lf(5  + b.u16(i + 1));
let list32_lf = lf(9  + b.u32(i + 1));
let list64_lf = lf(17 + b.u64(i + 1));

let u8_tvlf  = lf(b.u8 (i + 1));
let u16_tvlf = lf(b.u16(i + 1));
let u32_tvlf = lf(b.u32(i + 1));
let u64_tvlf = lf(b.u64(i + 1));

let fixutf8_lf   = lf(1 + (b.u8(i) - 0x20));
let fixbytes_lf  = lf(1 + (b.u8(i) - 0x30));
let fixtuple8_lf = lf(1 + b.u8(i + 1));
let fixint_lf    = l1;

let idx16_lf = lf(1 + 2 + b.u16(i + 1));
let idx32_lf = lf(1 + 4 + b.u32(i + 1));
let idx64_lf = lf(1 + 8 + b.u64(i + 1));

let bogus_lf = [](ibuf const &b, uint64_t i) -> uint64_t { throw BOGUS_LF_ERROR; };


// Typecode length functions
inline uint64_t tuple_tl(ibuf const &b, uint64_t i, uint64_t n)
{
  uint64_t l = 0;
  while (n--) l += b.tlen(i + l);
  return l;
}

let fixtuple_tlf = lf(2 + tuple_tl(b, i + 2, b.u8(i) - 0x48));


// Typecode value length functions
let fixutf8_tvlf   = lf(b.u8(i) - 0x20);
let fixbytes_tvlf  = lf(b.u8(i) - 0x30);
let fixtuple8_tvlf = u8_tvlf;


// Bytecode decode tables
lfn const lfns[256] =
{
  // 0x00 - 0x0f
  l2, l3, l5, l9,
  l2, l3, l5, l9,
  l5, l9, l9, l9,
  l1, l1, l1, bogus_lf,

  // 0x10 - 0x1f
  l1, l1, l1, l1,
  l1,  // tau
  l9,  // rho
  l9,  // theta
  bogus_lf,

  str8_lf, str16_lf, str32_lf, str64_lf,
  str8_lf, str16_lf, str32_lf, str64_lf,

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
  list8_lf, list16_lf, list32_lf, list64_lf,
  list8_lf, list16_lf, list32_lf, list64_lf,
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
  lf(3  + tuple_tl(b, i + 3,  b.u8 (i + 2))),
  lf(5  + tuple_tl(b, i + 5,  b.u16(i + 3))),
  lf(9  + tuple_tl(b, i + 9,  b.u32(i + 5))),
  lf(17 + tuple_tl(b, i + 17, b.u64(i + 9))),

  lf(3  + b.tlen(i + 3)),
  lf(5  + b.tlen(i + 5)),
  lf(9  + b.tlen(i + 9)),
  lf(17 + b.tlen(i + 17)),

  fixtuple_tlf, fixtuple_tlf, fixtuple_tlf, fixtuple_tlf,
  fixtuple_tlf, fixtuple_tlf, fixtuple_tlf, fixtuple_tlf,

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
  u8_tvlf, u16_tvlf, u32_tvlf, u64_tvlf,
  u8_tvlf, u16_tvlf, u32_tvlf, u64_tvlf,

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
  u8_tvlf, u16_tvlf, u32_tvlf, u64_tvlf,
  u8_tvlf, u16_tvlf, u32_tvlf, u64_tvlf,
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


tpacked lfnos[256];
tpacked tlfnos[256];
tpacked tvlfnos[256];


uint64_t const lfn_base = reinterpret_cast<uint64_t>(&l1);


void init_lfn_tables()
{
  pack_table(reinterpret_cast<uint64_t const*>(lfns),   lfn_base, lfnos,   256);
  pack_table(reinterpret_cast<uint64_t const*>(tlfns),  lfn_base, tlfnos,  256);
  pack_table(reinterpret_cast<uint64_t const*>(tvlfns), lfn_base, tvlfnos, 256);
}


#undef lf

}

inline uint64_t ibuf::len  (uint64_t i) const { return reinterpret_cast<lfn>(lfn_base + lfnos  [xs[i]])(*this, i); }
inline uint64_t ibuf::tlen (uint64_t i) const { return reinterpret_cast<lfn>(lfn_base + tlfnos [xs[i]])(*this, i); }
inline uint64_t ibuf::tvlen(uint64_t i) const { return reinterpret_cast<lfn>(lfn_base + tvlfnos[xs[i]])(*this, i); }


enum val_type : uint8_t
{
  UINT     = 0x00,
  INT      = 0x04,
  FLOAT32  = 0x08,
  FLOAT64  = 0x09,
  SYMBOL   = 0x0a,
  PIDFD    = 0x0b,
  BOOL     = 0x0c,
  NULLTYPE = 0x0e,

  ALPHA    = 0x10,
  OMEGA    = 0x11,
  IOTA     = 0x12,
  KAPPA    = 0x13,
  TAU      = 0x14,
  RHO      = 0x15,
  THETA    = 0x16,

  UTF8     = 0x18,
  BYTES    = 0x1c,

  TUPLE    = 0x20,
  ARRAY    = 0x24,
  INDEX    = 0x30,

  BOGUS    = 0x3f,
};

typedef uint64_t val_type_mask;


namespace  // Type dispatch tables
{

val_type const bts[256] =
{
  // 0x00-0x0f
  UINT, UINT, UINT, UINT,
  INT,  INT,  INT,  INT,
  FLOAT32, FLOAT64, SYMBOL, PIDFD,
  BOOL, BOOL, NULLTYPE, BOGUS,

  // 0x10-0x1f
  ALPHA, OMEGA, IOTA, KAPPA,
  TAU, RHO, THETA, BOGUS,
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
  INDEX, INDEX, INDEX, INDEX,
  INDEX, INDEX, INDEX, INDEX,
  INDEX, INDEX, INDEX, INDEX,
  INDEX, INDEX, INDEX, INDEX,

  // 0x60-0x6f
  BOGUS, BOGUS, BOGUS, BOGUS,
  INDEX, INDEX, INDEX, INDEX,
  INDEX, INDEX, INDEX, INDEX,
  INDEX, INDEX, INDEX, INDEX,

  // 0x70-0x7f
  BOGUS, BOGUS, BOGUS, BOGUS,
  INDEX, INDEX, INDEX, INDEX,
  INDEX, INDEX, INDEX, INDEX,
  INDEX, INDEX, INDEX, INDEX,

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


#define pf(body) [](ibuf const &b, uint64_t i) -> uint8_t const* { return (body); }

typedef uint8_t const*(*pfn)(ibuf const &, uint64_t);


let bogus_pf = [](ibuf const &b, uint64_t i) -> uint8_t const* { throw BOGUS_PF_ERROR; };

let p1  = pf(b + (i + 1));
let p2  = pf(b + (i + 2));
let p3  = pf(b + (i + 3));
let p4  = pf(b + (i + 4));
let p5  = pf(b + (i + 5));
let p6  = pf(b + (i + 6));
let p9  = pf(b + (i + 9));
let p10 = pf(b + (i + 10));
let p17 = pf(b + (i + 17));

let i16_pf = pf(b + (i + 5  + b.tlen(i + 5)));
let i32_pf = pf(b + (i + 9  + b.tlen(i + 9)));
let i64_pf = pf(b + (i + 17 + b.tlen(i + 17)));


pfn const sfns[256] =
{
  // 0x00-0x0f
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,

  // 0x10-0x1f
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  p2, p3, p5, p9,
  p2, p3, p5, p9,

  // 0x20-0x2f
  p1, p1, p1, p1,
  p1, p1, p1, p1,
  p1, p1, p1, p1,
  p1, p1, p1, p1,

  // 0x30-0x3f
  p1, p1, p1, p1,
  p1, p1, p1, p1,
  p1, p1, p1, p1,
  p1, p1, p1, p1,

  // 0x40-0x4f
  p3, p5, p9, p17,
  pf(b + (i + 3  + b.tlen(i + 3))),
  pf(b + (i + 5  + b.tlen(i + 5))),
  pf(b + (i + 9  + b.tlen(i + 9))),
  pf(b + (i + 17 + b.tlen(i + 17))),
  p2, p2, p2, p2,
  p2, p2, p2, p2,

  // 0x50-0x5f
  p4, p6, p10, bogus_pf,
  i16_pf, i16_pf, i16_pf, i16_pf,
  i16_pf, i16_pf, i16_pf, i16_pf,
  i16_pf, i16_pf, i16_pf, i16_pf,

  // 0x60-0x6f
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  i32_pf, i32_pf, i32_pf, i32_pf,
  i32_pf, i32_pf, i32_pf, i32_pf,
  i32_pf, i32_pf, i32_pf, i32_pf,

  // 0x70-0x7f
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  i64_pf, i64_pf, i64_pf, i64_pf,
  i64_pf, i64_pf, i64_pf, i64_pf,
  i64_pf, i64_pf, i64_pf, i64_pf,

  // 0x80-0xbf
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,

  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,

  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,

  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,

  // 0xc0-0xff
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,

  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,

  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,

  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
};

tpacked sfnos[256];

uint64_t const sfn_base = reinterpret_cast<uint64_t>(&p1);


void init_sfn_tables()
{
  pack_table(reinterpret_cast<uint64_t const *>(sfns), sfn_base, sfnos, 256);
}


#undef pf

}


struct sym
{
  uint64_t h;
  sym(std::string const &s) : h(std::hash<std::string>{}(s)) {}
  sym(uint64_t h_)          : h(h_)                          {}
  operator uint64_t() const { return h; }
};


struct hash
{
  uint64_t h;
  hash(uint64_t h_) : h(h_) {}
  operator uint64_t() const { return h; }
};


struct pidfd
{
  uint32_t pid;
  uint32_t fd;

  pidfd(uint32_t pid_, uint32_t fd_) : pid(pid_), fd(fd_) {}

  operator uint64_t() const { return (uint64_t) pid << 32 | fd; }

  bool operator<(pidfd const &v) const { return pid < v.pid || pid == v.pid && fd < v.fd; }
  bool operator>(pidfd const &v) const { return pid > v.pid || pid == v.pid && fd > v.fd; }
};


namespace  // Primitive type << operators
{

std::ostream &operator<<(std::ostream &s, sym   const &y) { return s << 's' << reinterpret_cast<void*>(y.h); }
std::ostream &operator<<(std::ostream &s, hash  const &h) { return s << 'h' << reinterpret_cast<void*>(h.h); }
std::ostream &operator<<(std::ostream &s, pidfd const &p) { return s << "[p=" << p.pid << ",fd=" << p.fd << "]"; }

}


struct tval
{
  ibuf const * const b;
  uint64_t     const i;
  uint64_t     const e;

  tval(ibuf const &b_, uint64_t i_) : b(&b_), i(i_), e(b->tlen(i)) {}


  struct it
  {
    ibuf const * const b;
    uint64_t           i;

    tval operator* ()            const { return tval(*b, i); }
    it & operator++()                  { i += b->tlen(i); return *this; }
    bool operator==(it const &x) const { return b == x.b && i == x.i; }
  };


  it begin() const
    { if (type() != TUPLE) throw INVALID_TYPE_ERROR;
      if (typecode() >= 0x48 && typecode() <= 0x4f) return it{b, i + 2};
      switch (typecode())
      {
      case 0x40: return it{b, i + 3};
      case 0x41: return it{b, i + 5};
      case 0x42: return it{b, i + 9};
      case 0x43: return it{b, i + 17};
      default: throw INTERNAL_ERROR;
      } }

  it end() const { return it{b, e}; }


  uint8_t  typecode() const { return b->u8(i); }
  val_type type()     const { return bts[typecode()]; }
  uint64_t vsize()    const { return b->tvlen(i); }
  uint64_t tsize()    const { return b->tlen(i); }


  uint64_t len() const
    { if (type() != ARRAY && type() != TUPLE) throw INVALID_TYPE_ERROR;
      if (typecode() >= 0x48 && typecode() <= 0x4f) return typecode() - 0x48;
      switch (typecode())
      {
      case 0x40: case 0x44: return b->u8(i + 2);
      case 0x41: case 0x45: return b->u16(i + 3);
      case 0x42: case 0x46: return b->u32(i + 5);
      case 0x43: case 0x47: return b->u64(i + 9);
      default: throw INTERNAL_ERROR;
      } }

  tval atype() const
    { if (type() != ARRAY) throw INVALID_TYPE_ERROR;
      switch (typecode())
      {
      case 0x44: return tval(*b, i + 3);
      case 0x45: return tval(*b, i + 5);
      case 0x46: return tval(*b, i + 9);
      case 0x47: return tval(*b, i + 17);
      default: throw INTERNAL_ERROR;
      } }


  // NOTE: this implementation requires that all typecodes are canonicalized
  // before being emitted to bytecode (otherwise comparisons will be unstable)
  int compare(tval const &t) const
    { let n1 =   tsize();
      let n2 = t.tsize();
      return n1 > n2 ? 1 : n1 < n2 ? -1 : std::__memcmp(*b + i, *t.b + t.i, n1); }


  uint64_t h() const { return xxh(b + i, e - i, 1); }
};


// Tagging bit schema:
// type:8 | owned:1 | immediate:1
constexpr inline uint64_t tagify  (val_type t, bool own = false) { return static_cast<uint64_t>(t) << 2 | (own ? 2 : 0) | 1; }
constexpr inline val_type tag_type(uint64_t tag)                 { return static_cast<val_type>(tag >> 2); }


// utf9 value
// Instances of this fit into two machine words on 64-bit machines.
// As a result, we can pass them around by-value with no appreciable
// overhead (although we unfortunately have a destructor to consider).
struct val
{
  // ibuf const * will be 8-byte aligned, so tags with the low bit set are
  // type designators; see tagify() and tag_type() above.
  union
  { ibuf const * b;
    uint64_t     tag; } const;

  union
  { uint64_t                               i;
    uint64_t                               vu;
    int64_t                                vi;
    double                                 vd;
    float                                  vf;
    pidfd                                  vp;
    sym                                    vy;
    std::basic_string_view<uint8_t> const *vb;
    std::vector<val>                      *vt; } const;


  val(ibuf const &b_, uint64_t i_) : b(&b_), i(i_)
    { if (reinterpret_cast<uint64_t>(b) & 7) throw ALIGNMENT_ERROR;
      b->check(b->len(i)); }

  val(uint64_t vu_) : tag(tagify(UINT)),    vu(vu_) {}
  val(int64_t vi_)  : tag(tagify(INT)),     vi(vi_) {}
  val(double vd_)   : tag(tagify(FLOAT64)), vd(vd_) {}
  val(float vf_)    : tag(tagify(FLOAT32)), vf(vf_) {}
  val(sym vy_)      : tag(tagify(SYMBOL)),  vy(vy_) {}
  val(pidfd vp_)    : tag(tagify(PIDFD)),   vp(vp_) {}

  val(std::vector<val> *vt_) : tag(tagify(TUPLE, true)), vt(vt_) {}

  val(std::string &s)
    : tag(tagify(UTF8, true)),
      vb(reinterpret_cast<std::basic_string_view<uint8_t>*>(
           new std::string_view(s.begin(), s.end())))
    {}

  // TODO: immediate constructors from std::string, std::vector, etc

  val(tval t_, ibuf const &b, uint64_t i)
    { switch (t_.typecode())
      {
      case 0x00: tag = tagify(UINT); vu = b.u8 (i); break;
      case 0x01: tag = tagify(UINT); vu = b.u16(i); break;
      case 0x02: tag = tagify(UINT); vu = b.u32(i); break;
      case 0x03: tag = tagify(UINT); vu = b.u64(i); break;

      case 0x04: tag = tagify(INT); vi = b.i8 (i); break;
      case 0x05: tag = tagify(INT); vi = b.i16(i); break;
      case 0x06: tag = tagify(INT); vi = b.i32(i); break;
      case 0x07: tag = tagify(INT); vi = b.i64(i); break;

      case 0x08: tag = tagify(FLOAT32); vf = b.f32(i); break;
      case 0x09: tag = tagify(FLOAT64); vd = b.f64(i); break;

      case 0x0a: tag = tagify(SYMBOL); vy = sym{b.u64(i)}; break;
      case 0x0b: tag = tagify(PIDFD);  vp = pidfd{b.u32(i), b.u32(i + 4)}; break;

      case 0x20: case 0x21: case 0x22: case 0x23:
      case 0x24: case 0x25: case 0x26: case 0x27:
      case 0x28: case 0x29: case 0x2a: case 0x2b:
      case 0x2c: case 0x2d: case 0x2e: case 0x2f:
      case 0x18:
      case 0x19:
      case 0x1a:
      case 0x1b: tag = tagify(UTF8, true); vb = new std::basic_string_view<uint8_t>(b + i, t_.vsize()); break;

      case 0x30: case 0x31: case 0x32: case 0x33:
      case 0x34: case 0x35: case 0x36: case 0x37:
      case 0x38: case 0x39: case 0x3a: case 0x3b:
      case 0x3c: case 0x3d: case 0x3e: case 0x3f:
      case 0x1c:
      case 0x1d:
      case 0x1e:
      case 0x1f: tag = tagify(BYTES, true); vb = new std::basic_string_view<uint8_t>(b + i, t_.vsize()); break;

      case 0x48: case 0x49: case 0x4a: case 0x4b:
      case 0x4c: case 0x4d: case 0x4e: case 0x4f:
      case 0x40:
      case 0x41:
      case 0x42:
      case 0x43:
        tag = tagify(TUPLE, true);
        vt = new std::vector<val>;
        for (let &t : t_)
        { vt->push_back(val(t, b, i));
          i += t.vsize(); }
        break;

      case 0x44:
      case 0x45:
      case 0x46:
      case 0x47:
        tag = tagify(ARRAY, true);
        vt = new std::vector<val>;
        for (uint64_t j = 0; j < t_.len(); j++)
        { vt->push_back(val(t_.atype(), b, i));
          i += t_.atype().vsize(); }
        break;

      default: throw INVALID_TYPE_ERROR;
      } }


  ~val()
    { switch (tag)
      {
      case tagify(UTF8,  true): case tagify(BYTES, true): delete vb; break;
      case tagify(TUPLE, true): case tagify(ARRAY, true): delete vt; break;
      } }


  bool     has_ibuf()                    const { return !(tag & 1); }
  bool     is_immediate()                const { return !has_ibuf(); }
  val_type type()                        const { return has_ibuf() ? bts[b->u8(i)] : tag_type(tag); }
  void     require_ibuf()                const { if (!has_ibuf()) throw IBUF_REQUIRED_ERROR; }
  void     require_type(val_type_mask m) const { if (!(1ull << type() & m)) throw INVALID_TYPE_ERROR; }


  val list() const
    { require_ibuf();
      uint64_t j = i;
      while (bts[b->cu8(j)] == INDEX) j += b->len(j);
      return j; }

  uint8_t const *mbegin() const { require_ibuf(); return reinterpret_cast<pfn>(sfn_base + sfnos[b->u8(i)])(*b, i); }
  uint8_t const *mend()   const { require_ibuf(); return *b + b->len(i); }
  uint64_t       mlen()   const { require_ibuf(); return mend() - mbegin(); }
  uint64_t       msize()  const { require_ibuf(); return b->len(i); }


  // TODO: this needs to be polymorphic to handle immediate vals and arrays
  struct it
  {
    ibuf const * const b;
    uint64_t           i;

    val  operator* ()              const { return val(*b, i); }
    it & operator++()                    { i += b->len(i); return *this; }
    bool operator==(it const &rhs) const { return b == rhs.b && i == rhs.i; }
  };

  it begin() const { require_ibuf(); return it{b, static_cast<uint64_t>(mbegin() - b->xs)}; }
  it end()   const { require_ibuf(); return it{b, i + b->len(i)}; }


  uint64_t len() const
    { switch (tag)
      {
      case tagify(UTF8,  true):
      case tagify(BYTES, true): return vb->size();

      case tagify(ARRAY, true):
      case tagify(TUPLE, true): return vt->size();

      default:
      {
        let x = b->u8(i);
        if (x >= 0x48 && x <= 0x4f) return x - 0x48;
        if (x >= 0x54 && x <= 0x5f) return b->u16(i + 1) >> 1;
        if (x >= 0x64 && x <= 0x6f) return b->u32(i + 1) >> 2;
        if (x >= 0x74 && x <= 0x7f) return b->u64(i + 1) >> 3;
        switch (x)
        {
        case 0x18: case 0x1c: return b->u8 (i + 1);
        case 0x19: case 0x1d: return b->u16(i + 1);
        case 0x1a: case 0x1e: return b->u32(i + 1);
        case 0x1b: case 0x1f: return b->u64(i + 1);

        case 0x40: case 0x44: return b->u8 (i + 2);
        case 0x41: case 0x45: return b->u16(i + 2);
        case 0x42: case 0x46: return b->u32(i + 2);
        case 0x43: case 0x47: return b->u64(i + 2);

        case 0x50: return b->u16(i + 1) >> 1;
        case 0x51: return b->u32(i + 1) >> 2;
        case 0x52: return b->u64(i + 1) >> 3;
        case 0x53: return 0;

        default: throw INVALID_TYPE_ERROR;
        } }
      } }


  uint64_t astride()        const { return atype().vsize(); }
  uint64_t asub(uint64_t i) const { return mbegin() + astride() * i - b->xs; }

  tval atype() const
    { require_ibuf();
      require_type(1ull << ARRAY);
      switch (b->u8(i))
      {
      case 0x44: return tval(*b, i + 3);
      case 0x45: return tval(*b, i + 5);
      case 0x46: return tval(*b, i + 9);
      case 0x47: return tval(*b, i + 17);
      default: throw INTERNAL_ERROR;
      } }


  operator uint64_t() const
    { if (type() == INT) throw SIGNED_COERCION_ERROR;
      require_type(1ull << UINT | 1ull << RHO | 1ull << THETA);
      if (!has_ibuf()) return vu;
      let x = b->u8(i);
      switch (x)
      {
      case 0x00: return b->u8 (i + 1);
      case 0x01: return b->u16(i + 1);
      case 0x02: return b->u32(i + 1);
      case 0x03: return b->u64(i + 1);
      default: throw INTERNAL_ERROR;
      } }

  operator int64_t() const
    { if (type() == UINT) throw SIGNED_COERCION_ERROR;
      require_type(1ull << INT);
      if (!has_ibuf()) return vi;
      let x = b->u8(i);
      if (x >= 0x80) return x - 0x80;
      switch (x)
      {
      case 0x04: return b->i8 (i + 1);
      case 0x05: return b->i16(i + 1);
      case 0x06: return b->i32(i + 1);
      case 0x07: return b->i64(i + 1);
      default: throw INTERNAL_ERROR;
      } }

  operator float() const
    { require_type(1ull << FLOAT32);
      return has_ibuf() ? b->f32(i + 1) : vf; }

  operator double() const
    { require_type(1ull << FLOAT64);
      return has_ibuf() ? b->f64(i + 1) : vd; }

  operator sym() const
    { require_type(1ull << SYMBOL);
      return has_ibuf() ? sym{b->u64(i + 1)} : vy; }

  operator pidfd() const
    { require_type(1ull << PIDFD);
      return has_ibuf() ? pidfd{b->u32(i + 1), b->u32(i + 5)} : vp; }

  operator bool() const
    { require_type(1ull << BOOL);
      return has_ibuf() ? b->u8(i) == 0x0d : !!vu; }

  operator std::string() const
    { require_type(1ull << UTF8 | 1ull << BYTES);
      return has_ibuf()
        ? std::string(reinterpret_cast<char const*>(mbegin()), mlen())
        : std::string(reinterpret_cast<char const*>(vb->begin(), vb->end())); }


  hash h() const
    { let t = type();
      switch (t)
      {

#define ht(ct)                                                          \
        { let x = ce(static_cast<uint64_t>(static_cast<ct>(*this)));    \
          return xxh(&x, sizeof(x), t); }

      case RHO:
      case THETA:
      case UINT:    ht(uint64_t)
      case INT:     ht(int64_t)
      case SYMBOL:  ht(sym)
      case PIDFD:   ht(pidfd)

#undef ht

      case FLOAT64:
      { let x = ce(static_cast<double>(*this));
        return xxh(&x, sizeof(x), t); }

      case FLOAT32:
      { let x = ce(static_cast<float>(*this));
        return xxh(&x, sizeof(x), t); }

      case BOOL: return xxh(NULL, 0, static_cast<bool>(*this) ? 0x0d : 0x0c);

      case NULLTYPE:
      case TAU:
      case ALPHA:
      case OMEGA:
      case IOTA:
      case KAPPA: return xxh(NULL, 0, t);

      case ARRAY:
      case UTF8:
      case BYTES: return xxh(mbegin(), mend() - mbegin(), t);

      case TUPLE:
      { uint64_t hs[len()];
        uint64_t i = 0;
        for (let &v : *this) hs[i++] = ce(v.h().h);
        return xxh(hs, sizeof(hs), t); }

      default: throw NOT_HASHABLE_ERROR;
      } }


  int compare(val const &v) const
    { let t1 =   type();
      let t2 = v.type();
      if (t1 != t2) throw COMPARE_ACROSS_TYPE_ERROR;
      switch (t1)
      {

#define cmpblock(t)                                     \
        { let x1 = static_cast<t>(*this);               \
          let x2 = static_cast<t>(v);                   \
          return x1 > x2 ? 1 : x1 < x2 ? -1 : 0; }

      case RHO:
      case THETA:
      case UINT:    cmpblock(uint64_t)
      case INT:     cmpblock(int64_t)
      case FLOAT32: cmpblock(float)
      case FLOAT64: cmpblock(double)
      case SYMBOL:  cmpblock(sym)
      case PIDFD:   cmpblock(pidfd)
      case BOOL:    cmpblock(bool)

#undef cmpblock

      case NULLTYPE:
      case ALPHA:
      case OMEGA:
      case KAPPA:
      case IOTA:
      case TAU:     return 0;

      case UTF8:
      case BYTES:
      case ARRAY:
      { if (let tc = atype().compare(v.atype())) throw COMPARE_ACROSS_ARRAY_TYPE_ERROR;
        let n1 =   mend() -   mbegin();
        let n2 = v.mend() - v.mbegin();
        if (let c = std::__memcmp(mbegin(), v.mbegin(), std::min(n1, n2))) return c;
        return n1 > n2 ? 1 : n1 < n2 ? -1 : 0; }

      case TUPLE:
      { bool m1 = false,
             m2 = false;
        for (it i1 = begin(), i2 = v.begin(), e1 = end(), e2 = v.end();
             m1 = i1 != e1, m2 = i2 != e2, m1 && m2;
             ++i1, ++i2)
          if (let c = (*i1).compare(*i2)) return c;
        return m1 ? 1 : m2 ? -1 : 0; }

      default: throw NOT_COMPARABLE_ERROR;
      } }

  bool operator< (val const &v) const { return this->compare(v) < 0; }
  bool operator> (val const &v) const { return this->compare(v) > 0; }
  bool operator<=(val const &v) const { return this->compare(v) <= 0; }
  bool operator>=(val const &v) const { return this->compare(v) >= 0; }
  bool operator==(val const &v) const { return this->compare(v) == 0; }
  bool operator!=(val const &v) const { return this->compare(v) != 0; }
};


namespace  // val/tval << operators
{

std::ostream &operator<<(std::ostream &s, val_type t)
{
  switch (t)
  {
  case UINT:     return s << "uint";
  case INT:      return s << "int";
  case FLOAT32:  return s << "f32";
  case FLOAT64:  return s << "f64";
  case SYMBOL:   return s << "sym";
  case PIDFD:    return s << "pidfd";
  case BOOL:     return s << "bool";
  case NULLTYPE: return s << "null";

  case ALPHA: return s << "α";
  case OMEGA: return s << "ω";
  case IOTA:  return s << "ι";
  case KAPPA: return s << "κ";
  case TAU:   return s << "τ";
  case RHO:   return s << "ρ";
  case THETA: return s << "θ";

  case UTF8:  return s << "utf8";
  case BYTES: return s << "bytes";
  case TUPLE: return s << "tuple";
  case ARRAY: return s << "array";
  case INDEX: return s << "index";

  case BOGUS: return s << "bogus";

  default: return s << "??? " << static_cast<int>(t);
  }
}

std::ostream &operator<<(std::ostream &s, error e)
{
  switch (e)
  {
  case ALIGNMENT_ERROR:                 return s << "alignment error";
  case BOUNDS_ERROR:                    return s << "bounds error";
  case IBUF_REQUIRED_ERROR:             return s << "ibuf required error";
  case INVALID_TYPE_ERROR:              return s << "invalid type error";
  case COMPARE_ACROSS_TYPE_ERROR:       return s << "compare across type error";
  case COMPARE_ACROSS_ARRAY_TYPE_ERROR: return s << "compare across array type error";
  case NOT_COMPARABLE_ERROR:            return s << "not comparable error";
  case NOT_HASHABLE_ERROR:              return s << "not hashable error";
  case SIGNED_COERCION_ERROR:           return s << "signed coercion error";
  case BOGUS_PF_ERROR:                  return s << "bogus pf error";
  case BOGUS_LF_ERROR:                  return s << "bogus lf error";
  case INVALID_BYTECODE_ERROR:          return s << "invalid bytecode error";
  case INVALID_TYPECODE_ERROR:          return s << "invalid typecode error";
  case INTERNAL_ERROR:                  return s << "internal error";
  default:                              return s << "??? " << static_cast<int>(e);
  }
}

std::ostream &operator<<(std::ostream &s, tval const &t)
{
  switch (t.type())
  {
  case UINT:     return s << "u" << t.vsize() * 8;
  case INT:      return s << "i" << t.vsize() * 8;
  case FLOAT32:
  case FLOAT64:  return s << "f" << t.vsize() * 8;
  case SYMBOL:   return s << "s";
  case PIDFD:    return s << "p";

  case UTF8:     return s << "u[" << t.vsize() << "]";
  case BYTES:    return s << "b[" << t.vsize() << "]";
  case TUPLE:
  { bool first = true;
    s << "(";
    for (let &v : t)
    { if (first) first = false;
      else       s << ", ";
      s << v; }
    return s << ")"; }

  case ARRAY: return s << t.atype() << "[" << t.len() << "]";

  default: throw INVALID_TYPECODE_ERROR;
  }
}

std::ostream &operator<<(std::ostream &s, val const &t)
{
  return s << "TODO: << val";
}

}


// TODO: what kind of dispatch structure do we want for the val
// operation space? Probably an external table of short names, each
// a (val, val) -> val function.


void utf9_init()
{
  init_lfn_tables();
  init_sfn_tables();
}


#undef let

}


template<> struct std::hash<tau::utf9::val>
{ uint64_t operator()(tau::utf9::val const &v) { return v.h(); } };

template<> struct std::hash<tau::utf9::tval>
{ uint64_t operator()(tau::utf9::tval const &t) { return t.h(); } };


#endif  // ifndef utf9_h
