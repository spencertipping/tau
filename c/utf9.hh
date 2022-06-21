#ifndef utf9_h
#define utf9_h


#include <bit>
#include <cstdint>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include <byteswap.h>


namespace tau::utf9
{

#define let auto const


bool     ibuf_magic_init   = false;
uint64_t ibuf_ref_magic[2] = {0};


// Input buffer of bytes to be parsed as bytecode
struct ibuf
{
  uint64_t       magic[2];
  uint8_t const *xs;
  uint64_t       l;


  ibuf(uint8_t const *xs_, uint64_t l_) : xs(xs_), l(l_)
    { if (!ibuf_magic_init)
      { std::random_device                      rd;
        std::mt19937_64                         gen(rd());
        std::uniform_int_distribution<uint64_t> d;
        ibuf_ref_magic[0] = d(gen);
        ibuf_ref_magic[1] = d(gen);
        ibuf_magic_init   = true; }
      magic[0] = ibuf_ref_magic[0];
      magic[1] = ibuf_ref_magic[1]; }


  bool is_ibuf() const
    { return magic[0] == ibuf_ref_magic[0] && magic[1] == ibuf_ref_magic[1]; }


  bool has  (uint64_t i) const { return i < l; }
  void check(uint64_t i) const { if (!has(i)) throw std::length_error("OOBE"); }

  uint64_t len  (uint64_t i) const;
  uint64_t tlen (uint64_t i) const;
  uint64_t tvlen(uint64_t i) const;


  uint8_t const *operator+(uint64_t i) const { return xs + i; }


  uint8_t  u8 (uint64_t i) const { return xs[i]; }
  uint16_t u16(uint64_t i) const
    { if constexpr (std::endian::native == std::endian::little)
                     return __bswap_16(*(uint16_t*)(xs + i));
      else if constexpr (std::endian::native == std::endian::big)
                          return *(uint16_t*)(xs + i); }

  uint32_t u32(uint64_t i) const
    { if constexpr (std::endian::native == std::endian::little)
                     return __bswap_32(*(uint32_t*)(xs + i));
      else if constexpr (std::endian::native == std::endian::big)
                          return *(uint32_t*)(xs + i); }

  uint64_t u64(uint64_t i) const
    { if constexpr (std::endian::native == std::endian::little)
                     return __bswap_64(*(uint64_t*)(xs + i));
      else if constexpr (std::endian::native == std::endian::big)
                          return *(uint64_t*)(xs + i); }


  uint8_t  c8 (uint64_t i) const { check(i);     return u8(i);  }
  uint16_t c16(uint64_t i) const { check(i + 1); return u16(i); }
  uint32_t c32(uint64_t i) const { check(i + 3); return u32(i); }
  uint64_t c64(uint64_t i) const { check(i + 7); return u64(i); }
};


enum val_type
{
  REF     = 0,
  INT     = 1,
  FLOAT32 = 2,
  FLOAT64 = 3,
  SYMBOL  = 4,
  PIDFD   = 5,
  TAU     = 6,

  ALPHA   = 8,
  OMEGA   = 9,
  IOTA    = 10,
  KAPPA   = 11,

  UTF8    = 12,
  BYTES   = 13,
  TUPLE   = 14,
  ARRAY   = 15,
  INDEX   = 16,

  BOGUS   = 255,
};


namespace
{

val_type const bts[256] =
{
  // 0x00-0x0f
  INT, INT, INT, INT,
  INT, INT, INT, INT,
  FLOAT32, FLOAT64, SYMBOL, PIDFD,
  BOGUS, BOGUS, BOGUS, BOGUS,

  // 0x10-0x1f
  ALPHA, OMEGA, IOTA, KAPPA,
  TAU, TAU, BOGUS, BOGUS,
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


let bogus_pf =
  [](ibuf const &b, uint64_t i) -> uint8_t const*
    { throw std::invalid_argument("bogus"); };

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


#undef pf

}


struct val
{
  union
  { ibuf const * b;
    uint64_t     tag; };

  union
  { uint64_t i;
    uint64_t vi;
    double   vd;
    float    vf; };


  val(ibuf const &b_, uint64_t i_) : b(&b_),       i(i_)   {}
  val(uint64_t vi_)                : tag(INT),     vi(vi_) {}
  val(double vd_)                  : tag(FLOAT64), vd(vd_) {}
  val(float vf_)                   : tag(FLOAT32), vf(vf_) {}


  bool has_ibuf() const { return tag >= 256 && b->is_ibuf(); }
  val_type t()    const { return has_ibuf() ? bts[b->c8(i)] : static_cast<val_type>(tag); }


  val list() const
    { uint64_t j = i; while (bts[b->c8(j)] == INDEX) j += b->len(j); return j; }

  uint8_t const *begin() const { return sfns[b->c8(i)](*b, i); }
  uint8_t const *end()   const { return *b + b->len(i); }

};


typedef uint64_t(*lfn)(ibuf const &, uint64_t);


namespace
{

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
let str8_lf   = lf(2 + b.c8 (i + 1));
let str16_lf  = lf(3 + b.c16(i + 1));
let str32_lf  = lf(5 + b.c32(i + 1));
let str64_lf  = lf(9 + b.c64(i + 1));

let list8_lf  = lf(3  + b.c8 (i + 1));
let list16_lf = lf(5  + b.c16(i + 1));
let list32_lf = lf(9  + b.c32(i + 1));
let list64_lf = lf(17 + b.c64(i + 1));

let u8_tvlf  = lf(b.c8 (i + 1));
let u16_tvlf = lf(b.c16(i + 1));
let u32_tvlf = lf(b.c32(i + 1));
let u64_tvlf = lf(b.c64(i + 1));

let fixutf8_lf   = lf(1 + (b.c8(i) - 0x20));
let fixbytes_lf  = lf(1 + (b.c8(i) - 0x30));
let fixtuple8_lf = lf(1 + b.c8(i + 1));
let fixint_lf    = l1;

let idx16_lf = lf(1 + 2 + b.c16(i + 1));
let idx32_lf = lf(1 + 4 + b.c32(i + 1));
let idx64_lf = lf(1 + 8 + b.c64(i + 1));

let bogus_lf =
  [](ibuf const &b, uint64_t i) -> uint64_t
  { throw std::invalid_argument("bogus"); };


// Typecode length functions
inline uint64_t tuple_tl(ibuf const &b, uint64_t i, uint64_t n)
{
  uint64_t l = 0;
  while (n--) l += b.tlen(i + l);
  return l;
}

let fixtuple_tlf = lf(2 + tuple_tl(b, i + 2, b.c8(i) - 0x48));


// Typecode value length functions
let fixutf8_tvlf   = lf(b.c8(i) - 0x20);
let fixbytes_tvlf  = lf(b.c8(i) - 0x30);
let fixtuple8_tvlf = u8_tvlf;


// Bytecode decode tables
lfn const lfns[256] =   // 2kiB dcache footprint (common case)
{
  // 0x00 - 0x0f
  l2, l3, l5, l9,
  l2, l3, l5, l9,
  l5, l9, l9, l9,
  bogus_lf, bogus_lf, bogus_lf, bogus_lf,

  // 0x10 - 0x1f
  l1, l1, l1, l1,
  l1, l9, bogus_lf, bogus_lf,

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
  lf(1 + 2 + 1 + b.c16(i + 1)),
  lf(1 + 4 + 1 + b.c32(i + 1)),
  lf(1 + 8 + 1 + b.c64(i + 1)),
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

lfn const tlfns[256] =  // 640B dcache footprint (worst-case)
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
  lf(3  + tuple_tl(b, i + 3,  b.c8 (i + 2))),
  lf(5  + tuple_tl(b, i + 5,  b.c16(i + 3))),
  lf(9  + tuple_tl(b, i + 9,  b.c32(i + 5))),
  lf(17 + tuple_tl(b, i + 17, b.c64(i + 9))),

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

lfn const tvlfns[256] = // 640B dcache footprint (worst-case)
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


#undef lf

}


inline uint64_t ibuf::len  (uint64_t i) const { check(i); return lfns  [xs[i]](*this, i); }
inline uint64_t ibuf::tlen (uint64_t i) const { check(i); return tlfns [xs[i]](*this, i); }
inline uint64_t ibuf::tvlen(uint64_t i) const { check(i); return tvlfns[xs[i]](*this, i); }


#undef let

}


#endif  // ifndef utf9_h
