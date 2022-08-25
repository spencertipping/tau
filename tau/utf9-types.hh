#ifndef tau_utf9_types_h
#define tau_utf9_types_h


#include "debug.hh"
#include "numerics.hh"
#include "types.hh"

#include "begin.hh"

namespace tau
{


#if tau_debug_iostream
enum class u9t;
enum class u9s;
struct u9tm;

O &operator<<(O &s, u9t);
O &operator<<(O &s, u9s);
O &operator<<(O &s, u9tm);
#endif


enum class u9t
{
  i8      = 0,
  i16     = 1,
  i32     = 2,
  i64     = 3,
  u8      = 4,
  u16     = 5,
  u32     = 6,
  u64     = 7,

  f32     = 8,
  f64     = 9,
  c32     = 10,
  c64     = 11,
  b       = 12,
  symbol  = 13,
  stream  = 14,

  bytes   = 16,
  utf8    = 17,
  index   = 18,

  tuple   = 20,
  map     = 21,
  set     = 22,
  tensor  = 23,

  pidfd   = 24,
  heapref = 25
};


enum class u9s
{
  f1  = 0,
  f2  = 1,
  f4  = 2,
  f8  = 3,
  v8  = 4,
  v16 = 5,
  v32 = 6,
  v64 = 7,
};


constexpr uf8 operator|(u9t t, u9s s) { return Sc<uf8>(t) << 3 | Sc<uf8>(s); }
constexpr u9t u9ts_t   (uf8 x)        { return Sc<u9t>(x >> 3); }
constexpr u9s u9ts_s   (uf8 x)        { return Sc<u9s>(x  & 7); }

constexpr bool u9sv(u9s s) { return Sc<uN>(s) >= 4; }

constexpr uN u9sb(u9s s)
{
  switch (s)
  {
  case u9s::v8:  return 2;
  case u9s::v16: return 3;
  case u9s::v32: return 5;
  case u9s::v64: return 9;
  default:       return 1;
  }
}

constexpr u9s u9sq(uN s)
{
  return ou<u32>(s) ? u9s::v64
       : ou<u16>(s) ? u9s::v32
       : ou<u8>(s)  ? u9s::v16
       : s == 8     ? u9s::f8
       : s == 4     ? u9s::f4
       : s == 2     ? u9s::f2
       : s == 1     ? u9s::f1
       :              u9s::v8;
}


template<class T>
uN u9ws(T xs, uN i, u9t t, uN s)
{
  let q = u9sq(s);
  W<u8>(xs, i, t | q);
  switch (q)
  {
  case u9s::v8:  W<u8> (xs, i + 1, s); return 2;
  case u9s::v16: W<u16>(xs, i + 1, s); return 3;
  case u9s::v32: W<u32>(xs, i + 1, s); return 5;
  case u9s::v64: W<u64>(xs, i + 1, s); return 9;
  default:                             return 1;
  }
}


struct u9tm
{
  constexpr static u32 mask(Il<u9t> xs)
    { u32 m = 0;
      for (let x : xs) m |= Sc<u32>(1) << Sc<iN>(x);
      return m; }


  u32c m;

  constexpr u9tm(Il<u9t> xs) : m(mask(xs)) {}
  constexpr u9tm(u32 m_)     : m(m_)       {}

  constexpr bool operator[](u9t t) const { return m & Sc<u32>(1) << Sc<iN>(t); }
  constexpr u9t  operator()(u9t t) const { A((*this)[t], "expected " << *this << ", got " << t); return t; }

  constexpr u9tm operator~()       const { return ~m; }
  constexpr u9tm operator|(u9tm x) const { return m | x.m; }
  constexpr u9tm operator&(u9tm x) const { return m & x.m; }
  constexpr u9tm operator^(u9tm x) const { return m ^ x.m; }
};


constexpr u9tm const u9unsigned{u9t::u8, u9t::u16, u9t::u32, u9t::u64};
constexpr u9tm const u9signed  {u9t::i8, u9t::i16, u9t::i32, u9t::i64};
constexpr u9tm const u9ints    = u9unsigned | u9signed;
constexpr u9tm const u9reals   = u9ints  | u9tm{u9t::f32, u9t::f64};
constexpr u9tm const u9numbers = u9reals | u9tm{u9t::c32, u9t::c64};

constexpr u9tm const u9strings {u9t::bytes, u9t::utf8};
constexpr u9tm const u9coll    {u9t::index, u9t::map, u9t::set, u9t::tuple, u9t::tensor};
constexpr u9tm const u9native  {u9t::pidfd, u9t::heapref};

constexpr u9tm const u9atomics = u9strings | u9numbers | u9tm{u9t::b, u9t::symbol};
constexpr u9tm const u9vectors = u9numbers;


constexpr uN u9logsizeof(u9t t)
{
  switch (t)
  {
  case u9t::i8:  case u9t::u8:                                return 0;
  case u9t::i16: case u9t::u16:                               return 1;
  case u9t::i32: case u9t::u32: case u9t::f32:                return 2;
  case u9t::i64: case u9t::u64: case u9t::f64: case u9t::c32: return 3;
  case u9t::c64:                                              return 4;
  default:                                                    return -1;
  }
}

constexpr uN u9sizeof(u9t t) { return 1 << u9logsizeof(t); }


#if tau_debug_iostream
O &operator<<(O &s, u9t t)
{
  switch (t)
  {
  case u9t::i8:  return s << "i8";
  case u9t::i16: return s << "i16";
  case u9t::i32: return s << "i32";
  case u9t::i64: return s << "i64";
  case u9t::u8:  return s << "u8";
  case u9t::u16: return s << "u16";
  case u9t::u32: return s << "u32";
  case u9t::u64: return s << "u64";

  case u9t::f32:    return s << "f32";
  case u9t::f64:    return s << "f64";
  case u9t::c32:    return s << "c32";
  case u9t::c64:    return s << "c64";
  case u9t::b:      return s << "b";
  case u9t::symbol: return s << "symbol";
  case u9t::stream: return s << "stream";

  case u9t::bytes: return s << "bytes";
  case u9t::utf8:  return s << "utf8";
  case u9t::index: return s << "index";

  case u9t::tuple:  return s << "tuple";
  case u9t::map:    return s << "map";
  case u9t::set:    return s << "set";
  case u9t::tensor: return s << "tensor";

  case u9t::pidfd:   return s << "pidfd";
  case u9t::heapref: return s << "heapref";

  default: return s << "reserved/invalid:" << Sc<uN>(t);
  }
}

O &operator<<(O &s, u9s x)
{
  switch (x)
  {
  case u9s::f1:  return s << "f1";
  case u9s::f2:  return s << "f2";
  case u9s::f4:  return s << "f4";
  case u9s::f8:  return s << "f8";
  case u9s::v8:  return s << "v8";
  case u9s::v16: return s << "v16";
  case u9s::v32: return s << "v32";
  case u9s::v64: return s << "v64";
  default:       return s << "invalid:" << Sc<uN>(x);
  }
}

O &operator<<(O &s, u9tm x)
{
  u32 m = x.m;
  s << "{";
  for (iN i = 0; m >> i; ++i)
  {
    if (m >> i & 1)
    {
      s << Sc<u9t>(i);
      if (m >> i + 1) s << ",";
    }
  }
  return s << "}";
}
#endif


}

#include "end.hh"


#endif
