#ifndef tau_utf9_lfn_h
#define tau_utf9_lfn_h


#include "ibuf.hh"

#include "../begin.hh"


namespace tau::utf9
{


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
let fixtuple8_lf = lf(2 + b.u8(i + 1));
let fixint_lf    = l1;

let idx16_lf = lf(1 + 2 + b.u16(i + 1));
let idx32_lf = lf(1 + 4 + b.u32(i + 1));
let idx64_lf = lf(1 + 8 + b.u64(i + 1));

let bogus_lf   = [](ibuf const &b, uint64_t i) -> uint64_t { throw decoding_error("bogus lf",   b, i); };
let bogus_tlf  = [](ibuf const &b, uint64_t i) -> uint64_t { throw decoding_error("bogus tlf",  b, i); };
let bogus_tvlf = [](ibuf const &b, uint64_t i) -> uint64_t { throw decoding_error("bogus tvlf", b, i); };


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
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,

  // 0x10-0x1f
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,
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
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,

  // 0x60-0x6f
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,

  // 0x70-0x7f
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,

  // 0x80-0xbf
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,

  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,

  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,

  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,

  // 0xc0-0xff
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,

  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,

  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,

  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,
  bogus_tlf, bogus_tlf, bogus_tlf, bogus_tlf,
};

lfn const tvlfns[256] =
{
  // 0x00-0x0f
  l1, l2, l4, l8,
  l1, l2, l4, l8,
  l4, l8, l8, l8,
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,

  // 0x10-0x1f
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,
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
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,

  // 0x60-0x6f
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,

  // 0x70-0x7f
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,

  // 0x80-0xbf
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,

  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,

  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,

  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,

  // 0xc0-0xff
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,

  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,

  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,

  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,
  bogus_tvlf, bogus_tvlf, bogus_tvlf, bogus_tvlf,
};


#undef lf

}


inline uint64_t ibuf::len  (uint64_t i) const { return lfns  [xs[i]](*this, i); }
inline uint64_t ibuf::tlen (uint64_t i) const { return tlfns [xs[i]](*this, i); }
inline uint64_t ibuf::tvlen(uint64_t i) const { return tvlfns[xs[i]](*this, i); }


}


#include "../end.hh"

#endif
