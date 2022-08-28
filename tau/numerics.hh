#ifndef tau_numerics_h
#define tau_numerics_h


#include <bit>
#include <limits>


#include "types.hh"


#include "begin.hh"

namespace tau
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


template<class I, class T = uN> inline constexpr bool ou(T x) { return !!(x >> sizeof(I) * 8); }
template<class I, class T = iN> inline constexpr bool oi(T x) { return x < Nl<I>::min() || x > Nl<I>::max(); }

template<class I, class T = uN> inline constexpr I cou(T x) { assert(!ou<I>(x)); return x; }
template<class I, class T = uN> inline constexpr I coi(T x) { assert(!oi<I>(x)); return x; }


// big-endian numeric IO
template<class T> struct twice;
template<class T> struct half;
template<class T> struct uns;

template<> struct twice<u8>  { typedef u16 t; };
template<> struct twice<u16> { typedef u32 t; };
template<> struct twice<u32> { typedef u64 t; };
template<> struct twice<i8>  { typedef i16 t; };
template<> struct twice<i16> { typedef i32 t; };
template<> struct twice<i32> { typedef i64 t; };

template<> struct half<u64>  { typedef u32 t; };
template<> struct half<u32>  { typedef u16 t; };
template<> struct half<u16>  { typedef u8  t; };
template<> struct half<i64>  { typedef i32 t; };
template<> struct half<i32>  { typedef i16 t; };
template<> struct half<i16>  { typedef i8  t; };

template<> struct uns<u64> { typedef u64 t; };
template<> struct uns<u32> { typedef u32 t; };
template<> struct uns<u16> { typedef u16 t; };
template<> struct uns<u8>  { typedef u8 t; };
template<> struct uns<i64> { typedef u64 t; };
template<> struct uns<i32> { typedef u32 t; };
template<> struct uns<i16> { typedef u16 t; };
template<> struct uns<i8>  { typedef u8 t; };


defRI(1) { return Sc<I>(xs[i]); }
defRI(2) { return Sc<I>(R<typename half<I>::t>(xs, i)) <<  8 | R<typename uns<typename half<I>::t>::t>(xs, i + 1); }
defRI(4) { return Sc<I>(R<typename half<I>::t>(xs, i)) << 16 | R<typename uns<typename half<I>::t>::t>(xs, i + 2); }
defRI(8) { return Sc<I>(R<typename half<I>::t>(xs, i)) << 32 | R<typename uns<typename half<I>::t>::t>(xs, i + 4); }

defWI(1) { xs[i] = x; }
defWI(2) { W<typename half<I>::t>(xs, i, x >> 8);  W<typename half<I>::t>(xs, i + 1, x & 0xff); }
defWI(4) { W<typename half<I>::t>(xs, i, x >> 16); W<typename half<I>::t>(xs, i + 2, x & 0xffff); }
defWI(8) { W<typename half<I>::t>(xs, i, x >> 32); W<typename half<I>::t>(xs, i + 4, x & 0xffffffff); }


defR(f32) { union {f32 f; i32 n;}; n = R<i32>(xs, i); return f; }
defR(f64) { union {f64 f; i64 n;}; n = R<i64>(xs, i); return f; }
defR(c32) { return c32{R<f32>(xs, i), R<f32>(xs, i + 4)}; }
defR(c64) { return c64{R<f64>(xs, i), R<f64>(xs, i + 8)}; }

defW(f32) { union {f32 f; i32 n;}; f = x; W(xs, i, n); }
defW(f64) { union {f64 f; i64 n;}; f = x; W(xs, i, n); }
defW(c32) { W<f32>(xs, i, x.real()); W<f32>(xs, i + 4, x.imag()); }
defW(c64) { W<f64>(xs, i, x.real()); W<f64>(xs, i + 8, x.imag()); }

template<class T> inline constexpr u8 bu(T x) { return ou<u32>(x) ? 3 : ou<u16>(x) ? 2 : ou<u8>(x) ? 1 : 0; }
template<class T> inline constexpr u8 bi(T x) { return oi<i32>(x) ? 3 : oi<i16>(x) ? 2 : oi<i8>(x) ? 1 : 0; }

template<class T> inline constexpr u8 su(T x) { return 1 << bu(x); }
template<class T> inline constexpr u8 si(T x) { return 1 << bi(x); }


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

#include "end.hh"


#endif
