#ifndef tau_utf9_lfn_h
#define tau_utf9_lfn_h


#include "../types.hh"

#include "error-proto.hh"
#include "ibuf.hh"


#include "../module/begin.hh"

namespace tau::utf9
{


namespace  // Length traversal dispatch
{

typedef uN(*lfn)(ibuf const &, uN);
#define lf(body) [](ibuf const &b, uN i) -> uN { return Sc<uN>(body); }


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
let l33 = lf(33);


// Value-length functions
let str8_lf   = lf(2 + b.U8 (i + 1));
let str16_lf  = lf(3 + b.U16(i + 1));
let str32_lf  = lf(5 + b.U32(i + 1));
let str64_lf  = lf(9 + b.U64(i + 1));

let list8_lf  = lf(3  + b.U8 (i + 1));
let list16_lf = lf(5  + b.U16(i + 1));
let list32_lf = lf(9  + b.U32(i + 1));
let list64_lf = lf(17 + b.U64(i + 1));

let u8_tvlf  = lf(b.U8 (i + 1));
let u16_tvlf = lf(b.U16(i + 1));
let u32_tvlf = lf(b.U32(i + 1));
let u64_tvlf = lf(b.U64(i + 1));

let fixutf8_lf   = lf(1 + (b.U8(i) - 0x20));
let fixbytes_lf  = lf(1 + (b.U8(i) - 0x30));
let fixtuple8_lf = lf(2 + b.U8(i + 1));
let fixint_lf    = l1;

let pidx8_lf  = lf(b.U8 (i + 1) + 4  +     (b.U8 (i + 2) >> b.U8(i + 3)));
let pidx16_lf = lf(b.U16(i + 1) + 6  + 2 * (b.U16(i + 3) >> b.U8(i + 5)));
let pidx32_lf = lf(b.U32(i + 1) + 10 + 4 * (b.U32(i + 5) >> b.U8(i + 9)));
let pidx64_lf = lf(b.U64(i + 1) + 18 + 8 * (b.U64(i + 9) >> b.U8(i + 17)));

let idx8_lf  = lf(b.U8 (i + 1) + 3  +     (~0ull >> b.U8(i + 2)));
let idx16_lf = lf(b.U16(i + 1) + 4  + 2 * (~0ull >> b.U8(i + 3)));
let idx32_lf = lf(b.U32(i + 1) + 6  + 4 * (~0ull >> b.U8(i + 5)));
let idx64_lf = lf(b.U64(i + 1) + 10 + 8 * (~0ull >> b.U8(i + 9)));

let hint_lf  = lf(1 + b.len(i + 1));

let bogus_lf   = [](ibuf const &b, uN i) -> uN { return throw_decoding_error<uN>("bogus lf",   b, i); };
let bogus_tlf  = [](ibuf const &b, uN i) -> uN { return throw_decoding_error<uN>("bogus tlf",  b, i); };
let bogus_tvlf = [](ibuf const &b, uN i) -> uN { return throw_decoding_error<uN>("bogus tvlf", b, i); };


// Typecode length functions
inline u64 tuple_tl(ibuf const &b, uN i, uN n)
{
  u64 l = 0;
  while (n--) l += b.tlen(i + l);
  return l;
}

let fixtuple_tlf = lf(2 + tuple_tl(b, i + 2, b.U8(i) - 0x48));


// Typecode value length functions
let fixutf8_tvlf   = lf(b.U8(i) - 0x20);
let fixbytes_tvlf  = lf(b.U8(i) - 0x30);
let fixtuple8_tvlf = u8_tvlf;


// Bytecode decode tables
lfn const lfns[256] =
{
  // 0x00 - 0x0f
  l2, l3, l5, l9,
  l2, l3, l5, l9,
  l5, l9, l33, l9,
  l1, l1, l1, bogus_lf,

  // 0x10 - 0x1f
  l1,  // α
  l1,  // ι
  l1,  // κ
  l5,  // ρ
  l5,  // θ
  l1,  // τ
  l1,  // ω
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
  pidx8_lf, pidx16_lf, pidx32_lf, pidx64_lf,
  idx8_lf,  idx16_lf,  idx32_lf,  idx64_lf,
  idx8_lf,  idx16_lf,  idx32_lf,  idx64_lf,
  hint_lf,  hint_lf,   hint_lf,   bogus_lf,

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
  lf(3  + tuple_tl(b, i + 3,  b.U8 (i + 2))),
  lf(5  + tuple_tl(b, i + 5,  b.U16(i + 3))),
  lf(9  + tuple_tl(b, i + 9,  b.U32(i + 5))),
  lf(17 + tuple_tl(b, i + 17, b.U64(i + 9))),

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


inline uN ibuf::len  (uN i) const { return lfns  [xs[i]](*this, i); }
inline uN ibuf::tlen (uN i) const { return tlfns [xs[i]](*this, i); }
inline uN ibuf::tvlen(uN i) const { return tvlfns[xs[i]](*this, i); }


}

#include "../module/end.hh"


#endif
