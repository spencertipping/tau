#ifndef tau_numerics_h
#define tau_numerics_h


#include <bit>


#include "types.hh"


#include "module/begin.hh"

namespace tau::numerics
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


template<class T> inline constexpr bool ou8 (T x) { return !!(x >> 8); }
template<class T> inline constexpr bool ou16(T x) { return !!(x >> 16); }
template<class T> inline constexpr bool ou32(T x) { return !!(x >> 32); }

/*
// FIXME: edge case (see static_asserts below)
// TODO: define these more carefully; what does a "signed overflow" mean?
template<class T> inline constexpr bool oi8 (T x) { let y = x >> 8; return y != 0 && y != -1; }
template<class T> inline constexpr bool oi16(T x) { return sizeof(T) > 2 && x << sizeof(T) * 8 - 16 >> sizeof(T) * 8 - 16 != x; }
template<class T> inline constexpr bool oi32(T x) { return sizeof(T) > 4 && x << sizeof(T) * 8 - 32 >> sizeof(T) * 8 - 32 != x; }

static_assert(!oi8<i64>(127));
static_assert( oi8<i64>(128));
static_assert(!oi8<i32>(127));
static_assert( oi8<i32>(128));
static_assert(!oi8<i16>(127));
static_assert( oi8<i16>(128));

static_assert(!oi16<i64>(32767));
static_assert( oi16<i64>(32768));
static_assert(!oi16<i32>(32767));
static_assert( oi16<i32>(32768));
static_assert(!oi16<i16>(32767));
static_assert( oi16<i16>(32768));
*/


inline constexpr u8  cou8 (u64 x) { assert(!ou8(x));  return x; }
inline constexpr u16 cou16(u64 x) { assert(!ou16(x)); return x; }
inline constexpr u32 cou32(u64 x) { assert(!ou32(x)); return x; }

//inline constexpr i8  coi8 (i64 x) { assert(!oi8(x));  return x; }
//inline constexpr i16 coi16(i64 x) { assert(!ou16(x)); return x; }
//inline constexpr i32 coi32(i64 x) { assert(!oi32(x)); return x; }


// big-endian numeric IO
template<class T> inline constexpr u8   U8 (T xs, uN i)        { return Sc<u8>(xs[i]); }
template<class T> inline constexpr void U8 (T xs, uN i, u8 x)  { xs[i] = x; }
template<class T> inline constexpr u16  U16(T xs, uN i)        { return Sc<u16>(xs[i]) << 8 | Sc<u16>(xs[i + 1]); }
template<class T> inline constexpr void U16(T xs, uN i, u16 x) { xs[i] = x >> 8; xs[i + 1] = x & 0xff; }
template<class T> inline constexpr u32  U32(T xs, uN i)        { return Sc<u32>(U16(xs, i)) << 16 | U16(xs, i + 2); }
template<class T> inline constexpr void U32(T xs, uN i, u32 x) { U16(xs, i, x >> 16); U16(xs, i, x & 0xffffu); }
template<class T> inline constexpr u64  U64(T xs, uN i)        { return Sc<u64>(U32(xs, i)) << 32 | U32(xs, i + 4); }
template<class T> inline constexpr void U64(T xs, uN i, u64 x) { U32(xs, i, x >> 32); U32(xs, i, x & 0xffffffffu); }

template<class T> inline constexpr i8   I8 (T xs, uN i)        { return Sc<i8>(xs[i]); }
template<class T> inline constexpr void I8 (T xs, uN i, i8 x)  { xs[i] = x; }
template<class T> inline constexpr i16  I16(T xs, uN i)        { return Sc<i16>(xs[i]) << 8 | Sc<i16>(xs[i + 1]); }
template<class T> inline constexpr void I16(T xs, uN i, i16 x) { xs[i] = x >> 8; xs[i + 1] = x & 0xff; }
template<class T> inline constexpr i32  I32(T xs, uN i)        { return Sc<i32>(I16(xs, i)) << 16 | I16(xs, i + 2); }
template<class T> inline constexpr void I32(T xs, uN i, i32 x) { I16(xs, i, x >> 16); I16(xs, i, x & 0xffff); }
template<class T> inline constexpr i64  I64(T xs, uN i)        { return Sc<i64>(I32(xs, i)) << 32 | I32(xs, i + 4); }
template<class T> inline constexpr void I64(T xs, uN i, i64 x) { I32(xs, i, x >> 32); I32(xs, i, x & 0xffffffff); }


// NOTE: assumes floats and ints undergo the same hardware endian conversion
// (which should be the case almost everywhere)

// TODO static_asserts for int/float endianness consistency
template<class T> inline constexpr f32  F32(T xs, uN i)        { union {f32 f; i32 n;}; n = I32(xs, i); return f; }
template<class T> inline constexpr void F32(T xs, uN i, f32 x) { union {f32 f; i32 n;}; f = x; I32(xs, i, n); }
template<class T> inline constexpr f64  F64(T xs, uN i)        { union {f64 f; i64 n;}; n = I64(xs, i); return f; }
template<class T> inline constexpr void F64(T xs, uN i, f64 x) { union {f64 f; i64 n;}; f = x; I64(xs, i, n); }


inline constexpr u8 bu(u64 x) { return ou32(x) ? 3 : ou16(x) ? 2 : ou8(x) ? 1 : 0; }
//inline constexpr u8 bi(i64 x) { return oi32(x) ? 3 : oi16(x) ? 2 : oi8(x) ? 1 : 0; }

inline constexpr u8 su(u64 x) { return 1 << bu(x); }
//inline constexpr u8 si(i64 x) { return 1 << bi(x); }


template<class T>
inline constexpr u8 ilog(T x)
{ u8 i = 0;
  for (u8 j = sizeof(T) * 4; j; j >>= 1) if (x >> j) i += j, x >>= j;
  return i; }

static_assert(ilog(1) == 0);
static_assert(ilog(2) == 1);
static_assert(ilog(4) == 2);
static_assert(ilog(7) == 2);
static_assert(ilog(8) == 3);


}

#include "module/end.hh"


#endif
