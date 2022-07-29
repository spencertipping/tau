#ifndef tau_utf9_pfn_h
#define tau_utf9_pfn_h


#include <cstdint>

#include "ibuf.hh"

#include "../module/begin.hh"


namespace tau::utf9
{


#define pf(body) [](ibuf const &b, uint64_t i) -> uint8_t const* { return (body); }

typedef uint8_t const*(*pfn)(ibuf const &, uint64_t);


let bogus_pf = [](ibuf const &b, uint64_t i) -> uint8_t const* { throw decoding_error("bogus pf", b, i); };

let p1  = pf(b + (i + 1));
let p2  = pf(b + (i + 2));
let p3  = pf(b + (i + 3));
let p4  = pf(b + (i + 4));
let p5  = pf(b + (i + 5));
let p6  = pf(b + (i + 6));
let p9  = pf(b + (i + 9));
let p10 = pf(b + (i + 10));
let p17 = pf(b + (i + 17));

// NOTE: index mbegin() points to the beginning of the tuple
let i16_pf = pf(b + (i + 5  + 4  * b.u16(i + 3)));
let i32_pf = pf(b + (i + 9  + 8  * b.u32(i + 5)));
let i64_pf = pf(b + (i + 17 + 16 * b.u64(i + 9)));


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
  p6,     p10,    bogus_pf, bogus_pf,
  i16_pf, i16_pf, i32_pf, i32_pf,
  i64_pf, i64_pf,

  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf,

  // 0x60-0x6f
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,

  // 0x70-0x7f
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,
  bogus_pf, bogus_pf, bogus_pf, bogus_pf,

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


#include "../module/end.hh"

#endif
