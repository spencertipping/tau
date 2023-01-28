#ifndef τnumerics_h
#define τnumerics_h


#include <bit>
#include <limits>


#include "types.hh"


#include "begin.hh"

namespace τ
{


let nτ = 6.28318530717959;
let nφ = 0.5 + sqrt(5) / 2;


letc LE = std::endian::native == std::endian::little;
letc BE = std::endian::native == std::endian::big;

static_assert(LE || BE, "unsupported endianness");


ic u16 bs16(u16 x) { return      x >> 8   | x                                         << 8; }
ic u32 bs32(u32 x) { return bs16(x >> 16) | Sc<u32>(bs16(x & 0xffffull))     << 16; }
ic u64 bs64(u64 x) { return bs32(x >> 32) | Sc<u64>(bs32(x & 0xffffffffull)) << 32; }

static_assert(bs16(0x1234u)               == 0x3412u);
static_assert(bs32(0x12345678u)           == 0x78563412u);
static_assert(bs64(0x0123456789abcdefull) == 0xefcdab8967452301ull);


ic u64 ce(u64 x) { return LE ? bs64(x) : x; }
ic u32 ce(u32 x) { return LE ? bs32(x) : x; }
ic u16 ce(u16 x) { return LE ? bs16(x) : x; }

ic i64 ce(i64 x) { return LE ? bs64(x) : x; }
ic i32 ce(i32 x) { return LE ? bs32(x) : x; }
ic i16 ce(i16 x) { return LE ? bs16(x) : x; }

ic f64 ce(f64 const x)
{
  if (BE) return x;
  union { f64 xd; u64 xi; }; xd = x;
  xi = bs64(xi);
  return xd;
}

ic f32 ce(f32 const x)
{
  if (BE) return x;
  union { f32 xf; u32 xi; }; xf = x;
  xi = bs32(xi);
  return xf;
}


#if defined(__GNUC__)
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wsign-compare"
#elif defined(__clang__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wsign-compare"
#endif

template<class I, class T = uN> ic bool ou(T x) { return !!(x >> sizeof(I) * 8); }
template<class I, class T = iN> ic bool oi(T x) { return x < Nl<I>::min() || x > Nl<I>::max(); }

#if defined(__GNUC__)
# pragma GCC diagnostic pop
#elif defined(__clang__)
# pragma clang diagnostic pop
#endif

template<class I, class T = uN> ic I cou(T x) { A(!ou<I>(x), x << " overflows [" << Nl<I>::min() << ", " << Nl<I>::max() << "]"); return x; }
template<class I, class T = uN> ic I coi(T x) { A(!oi<I>(x), x << " overflows [" << Nl<I>::min() << ", " << Nl<I>::max() << "]"); return x; }


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
template<> struct uns<u8>  { typedef u8  t; };
template<> struct uns<i64> { typedef u64 t; };
template<> struct uns<i32> { typedef u32 t; };
template<> struct uns<i16> { typedef u16 t; };
template<> struct uns<i8>  { typedef u8  t; };


defRI(1) { return Sc<I>(xs[i]); }
defRI(2) { return Sc<I>(R<typename half<I>::t>(xs, i)) <<  8 | R<typename uns<typename half<I>::t>::t>(xs, i + 1); }
defRI(4) { return Sc<I>(R<typename half<I>::t>(xs, i)) << 16 | R<typename uns<typename half<I>::t>::t>(xs, i + 2); }
defRI(8) { return Sc<I>(R<typename half<I>::t>(xs, i)) << 32 | R<typename uns<typename half<I>::t>::t>(xs, i + 4); }

defW(bool) { xs[i] = x; }
defW(i8)   { xs[i] = x; }
defW(u8)   { xs[i] = x; }
defW(i16)  { W(xs, i, Sc<i8> (x >> 8));  W(xs, i + 1, Sc<i8> (x & 0xff)); }
defW(u16)  { W(xs, i, Sc<u8> (x >> 8));  W(xs, i + 1, Sc<u8> (x & 0xff)); }
defW(i32)  { W(xs, i, Sc<i16>(x >> 16)); W(xs, i + 2, Sc<i16>(x & 0xffff)); }
defW(u32)  { W(xs, i, Sc<u16>(x >> 16)); W(xs, i + 2, Sc<u16>(x & 0xffff)); }
defW(i64)  { W(xs, i, Sc<i32>(x >> 32)); W(xs, i + 4, Sc<i32>(x & 0xffffffffl)); }
defW(u64)  { W(xs, i, Sc<u32>(x >> 32)); W(xs, i + 4, Sc<u32>(x & 0xffffffffl)); }


defR(f32) { union {f32 f; i32 n;}; n = R<i32>(xs, i); return f; }
defR(f64) { union {f64 f; i64 n;}; n = R<i64>(xs, i); return f; }
defR(c32) { return c32{R<f32>(xs, i), R<f32>(xs, i + 4)}; }
defR(c64) { return c64{R<f64>(xs, i), R<f64>(xs, i + 8)}; }

defW(f32) { union {f32 f; i32 n;}; f = x; W(xs, i, n); }
defW(f64) { union {f64 f; i64 n;}; f = x; W(xs, i, n); }
defW(c32) { W(xs, i, x.real()); W(xs, i + 4, x.imag()); }
defW(c64) { W(xs, i, x.real()); W(xs, i + 8, x.imag()); }


// NOTE: non-portable; pointers are written native-endian
defR(void*) { return Rc<void*>(R<uN>(xs, i)); }
defW(void*) { W(xs, i, Rc<uN>(x)); }


ic u8 bu(u64 x) { return ou<u32>(x) ? 3 : ou<u16>(x) ? 2 : ou<u8>(x) ? 1 : 0; }
ic u8 bi(i64 x) { return oi<i32>(x) ? 3 : oi<i16>(x) ? 2 : oi<i8>(x) ? 1 : 0; }

ic u8 su(u64 x) { return 1 << bu(x); }
ic u8 si(i64 x) { return 1 << bi(x); }


// Bytes required to store an integer
ic u8 ubytes(u64 x)
{ u8 r = 0;
  if (x >> 32) r += 4, x >>= 32;
  if (x >> 16) r += 2, x >>= 16;
  if (x >> 8)  r += 1, x >>= 8;
  if (x) ++r;
  return r; }

// Bits required to store an integer (i.e. index of highest bit)
ic u8 ubits(u64 x)
{ u8 i = 0;
  for (u8 j = 32; j; j >>= 1) if (x >> j) i += j, x >>= j;
  return i; }

static_assert(ubytes(0) == 0);
static_assert(ubytes(1) == 1);
static_assert(ubytes(255) == 1);
static_assert(ubytes(256) == 2);

static_assert(ubits(1) == 0);
static_assert(ubits(2) == 1);
static_assert(ubits(4) == 2);
static_assert(ubits(7) == 2);
static_assert(ubits(8) == 3);


}

#include "end.hh"


#endif
