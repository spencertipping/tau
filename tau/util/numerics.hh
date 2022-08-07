#ifndef tau_util_numerics_h
#define tau_util_numerics_h


#include <bit>


#include "../types.hh"


#include "../module/begin.hh"

namespace tau::util::numerics
{


let constexpr LE = std::endian::native == std::endian::little;
let constexpr BE = std::endian::native == std::endian::big;

static_assert(LE || BE, "unsupported endianness");


inline constexpr u16 bs16(u16 x) { return      x >> 8   | x                                         << 8; }
inline constexpr u32 bs32(u32 x) { return bs16(x >> 16) | static_cast<u32>(bs16(x & 0xffffull))     << 16; }
inline constexpr u64 bs64(u64 x) { return bs32(x >> 32) | static_cast<u64>(bs32(x & 0xffffffffull)) << 32; }

static_assert(bs16(0x1234u)               == 0x3412u);
static_assert(bs32(0x12345678u)           == 0x78563412u);
static_assert(bs64(0x0123456789abcdefull) == 0xefcdab8967452301ull);


inline constexpr u64 ce(u64 x) { return LE ? bs64(x) : x; }
inline constexpr u32 ce(u32 x) { return LE ? bs32(x) : x; }
inline constexpr u16 ce(u16 x) { return LE ? bs16(x) : x; }

inline constexpr i64 ce(i64 x) { return LE ? bs64(x) : x; }
inline constexpr i32 ce(i32 x) { return LE ? bs32(x) : x; }
inline constexpr i16 ce(i16 x) { return LE ? bs16(x) : x; }

inline constexpr f64 ce(f64 const x)
{
  if (BE) return x;
  union { f64 xd; u64 xi; }; xd = x;
  xi = bs64(xi);
  return xd;
}

inline constexpr f32 ce(f32 const x)
{
  if (BE) return x;
  union { f32 xf; u32 xi; }; xf = x;
  xi = bs32(xi);
  return xf;
}


inline constexpr bool ou8 (u64 x) { return !!(x >> 8); }
inline constexpr bool ou16(u64 x) { return !!(x >> 16); }
inline constexpr bool ou32(u64 x) { return !!(x >> 32); }

inline constexpr bool oi8 (i64 x) { return x << 56 >> 56 != x; }
inline constexpr bool oi16(i64 x) { return x << 48 >> 48 != x; }
inline constexpr bool oi32(i64 x) { return x << 32 >> 32 != x; }


inline constexpr u8  cou8 (u64 x) { assert(!ou8(x));  return x; }
inline constexpr u16 cou16(u64 x) { assert(!ou16(x)); return x; }
inline constexpr u32 cou32(u64 x) { assert(!ou32(x)); return x; }

inline constexpr i8  coi8 (i64 x) { assert(!oi8(x)); return x; }
inline constexpr i16 coi16(i64 x) { assert(!ou16(x)); return x; }
inline constexpr i32 coi32(i64 x) { assert(!oi32(x)); return x; }


inline constexpr u8 bu(u64 x) { return ou32(x) ? 3 : ou16(x) ? 2 : ou8(x) ? 1 : 0; }
inline constexpr u8 bi(i64 x) { return oi32(x) ? 3 : oi16(x) ? 2 : oi8(x) ? 1 : 0; }

inline constexpr u8 su(u64 x) { return 1 << bu(x); }
inline constexpr u8 si(i64 x) { return 1 << bi(x); }


template<class T>
inline constexpr u8 ilog(T x)
{ u8 i = 0;
  for (u8 j = sizeof(T) * 4; j; j >>= 1) if (x >> j) i += j, x >>= j;
  return i; }


}

#include "../module/end.hh"


#endif
