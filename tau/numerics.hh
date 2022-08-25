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

template<> struct twice<u8>  { typedef u16 type; };
template<> struct twice<u16> { typedef u32 type; };
template<> struct twice<u32> { typedef u64 type; };
template<> struct twice<i8>  { typedef i16 type; };
template<> struct twice<i16> { typedef i32 type; };
template<> struct twice<i32> { typedef i64 type; };

template<> struct half<u64>  { typedef u32 type; };
template<> struct half<u32>  { typedef u16 type; };
template<> struct half<u16>  { typedef u8  type; };
template<> struct half<i64>  { typedef i32 type; };
template<> struct half<i32>  { typedef i16 type; };
template<> struct half<i16>  { typedef i8  type; };


template<class I, class T>
inline constexpr
typename std::enable_if<std::is_integral<I>::value and sizeof(I) == 1, I>::type
R(T xs, uN i)
{ return Sc<I>(xs[i]); }

template<class I, class T> inline constexpr
typename std::enable_if<std::is_integral<I>::value and sizeof(I) == 2, I>::type
R(T xs, uN i)
{ return Sc<I>(R<typename half<I>::type>(xs, i)) << 8
       |       R<typename half<I>::type>(xs, i + 1); }

template<class I, class T>
inline constexpr
typename std::enable_if<std::is_integral<I>::value and sizeof(I) == 4, I>::type
R(T xs, uN i)
{ return Sc<I>(R<typename half<I>::type>(xs, i)) << 16
       |       R<typename half<I>::type>(xs, i + 2); }

template<class I, class T>
inline constexpr
typename std::enable_if<std::is_integral<I>::value and sizeof(I) == 8, I>::type
R(T xs, uN i)
{ return Sc<I>(R<typename half<I>::type>(xs, i)) << 32
       |       R<typename half<I>::type>(xs, i + 4); }


template<class I, class T>
inline constexpr
typename std::enable_if<std::is_integral<I>::value and sizeof(I) == 1, void>::type
W(T xs, uN i, I x)
{ xs[i] = x; }

template<class I, class T>
inline constexpr
typename std::enable_if<std::is_integral<I>::value and sizeof(I) == 2, void>::type
W(T xs, uN i, I x)
{ W<typename half<I>::type>(xs, i, x >> 8);
  W<typename half<I>::type>(xs, i + 1, x & 0xff); }

template<class I, class T>
inline constexpr
typename std::enable_if<std::is_integral<I>::value and sizeof(I) == 4, void>::type
W(T xs, uN i, I x)
{ W<typename half<I>::type>(xs, i, x >> 16);
  W<typename half<I>::type>(xs, i + 2, x & 0xffff); }

template<class I, class T>
inline constexpr
typename std::enable_if<std::is_integral<I>::value and sizeof(I) == 8, void>::type
W(T xs, uN i, I x)
{ W<typename half<I>::type>(xs, i, x >> 32);
  W<typename half<I>::type>(xs, i + 4, x & 0xffffffff); }


template<class I, class T>
inline constexpr
typename std::enable_if<std::is_same<I, f32>::value, f32>::type
R(T xs, uN i)
{ union {f32 f; i32 n;}; n = R<i32>(xs, i); return f; }

template<class I, class T>
inline constexpr
typename std::enable_if<std::is_same<I, f64>::value, f64>::type
R(T xs, uN i)
{ union {f64 f; i64 n;}; n = R<i64>(xs, i); return f; }


template<class I, class T>
inline constexpr
typename std::enable_if<std::is_same<I, f32>::value, void>::type
W(T xs, uN i, f32 x)
{ union {f32 f; i32 n;}; f = x; I32(xs, i, n); }

template<class I, class T>
inline constexpr
typename std::enable_if<std::is_same<I, f64>::value, void>::type
W(T xs, uN i, f64 x)
{ union {f64 f; i64 n;}; f = x; I64(xs, i, n); }


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
