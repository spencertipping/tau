#ifndef τutf9_types_h
#define τutf9_types_h


#include "../dep/picosha2.h"


#include "ctypes.hh"
#include "debug.hh"
#include "numerics.hh"
#include "types.hh"

#include "begin.hh"

namespace τ
{


#if τdebug_iostream
  enum class u9t;
  enum class u9s;
  enum class u9st;
  struct     u9tm;

  O &operator<<(O &s, u9t);
  O &operator<<(O &s, u9s);
  O &operator<<(O &s, u9st);
  O &operator<<(O &s, u9tm);
#endif


template<class T> struct u9_sizeof { sletc v = sizeof(T); };


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

  pi      = 24,
  phi     = 25,
  host    = 26,
  build   = 27,

  none    = 30,  // not a real type
  frame   = 31,
};


enum class u9st
{
  ω = 1,
  τ = 2,
  θ = 3,
  ι = 4,
  κ = 5,
};


static_assert(sizeof(int) <= sizeof(u32));

struct u9_stream { u9st t; u64 n; };


sletc u9ω = u9_stream{u9st::ω, 0};
sletc u9τ = u9_stream{u9st::τ, 0};
sletc u9κ = u9_stream{u9st::κ, 0};

ic u9_stream u9θ(u64 x) { return u9_stream{u9st::θ, x}; }
ic u9_stream u9θ(f64 x) { return u9_stream{u9st::θ, Sc<u64>(x * Sc<f64>(Nl<u64>::max()))}; }
ic u9_stream u9ι(u64 x) { return u9_stream{u9st::ι, x}; }


typedef u8 u9_scoped_t;


enum class u9_π : u9_scoped_t
{
  ref = 1,
};

enum class u9_Φ : u9_scoped_t
{
  heapref = 1,
  heappin = 2,
  fd      = 3,
};

enum class u9_host : u9_scoped_t
{
  path = 1,
  pid  = 2,
};

enum class u9_build : u9_scoped_t
{
  istruct = 1,
};


template<class E, class T>
struct u9_scoped
{
  typedef E Et;
  typedef T Tt;

  static_assert(sizeof(E) == sizeof(u9_scoped_t));
  E t;
  T x;
};

// NOTE: required to defeat auto-struct-alignment (which we don't have
// in UTF9 encodings)
template<class E, class T> struct u9_sizeof<u9_scoped<E, T>> { sletc v = sizeof(E) + sizeof(T); };

template<class T>          struct is_u9_scoped                  { sletc v = false; };
template<class E, class T> struct is_u9_scoped<u9_scoped<E, T>> { sletc v = true; };


template<class J, class X>
ic typename std::enable_if<is_u9_scoped<J>::v, J>::type
R(X xs, uN i)
{ return u9_scoped<typename J::Et, typename J::Tt>
    {Sc<typename J::Et>(R<u9_scoped_t>(xs, i)),
     R <typename J::Tt>               (xs, i + 1)}; }

template<class J, class X>
ic typename std::enable_if<is_u9_scoped<J>::v, void>::type
W(X xs, uN i, J const &x)
{ W(xs, i,     Sc<u9_scoped_t>(x.t));
  W(xs, i + 1, x.x); }


enum class u9_none : u8
{
  generic            = 0,
  key_lookup_missing = 1,
  tuple_bounds_error = 2,
};

defR(u9_none) { return u9_none{R<u8>(xs, i)}; }
defW(u9_none) { W(xs, i, Sc<u8>(x)); }


struct u9_symbol
{
  uN h;
  Bc x;

  u9_symbol(B &&x_) : h(std::hash<B>{}(x_)), x(std::move(x_)) {}
  u9_symbol(B &x_)  : h(std::hash<B>{}(x_)), x(x_) {}

  static u9_symbol gensym()
    { static B g;
      return u9_symbol{g = sha(g).x.substr(0, 16)}; }

  template<class T>
  static u9_symbol sha(T const &s)
    { B b;
      b.resize(picosha2::k_digest_size);
      picosha2::hash256(s, b);
      return u9_symbol{std::move(b)}; }

  template<class T>
  static u9_symbol str(T const &s)
    { return u9_symbol{B(Rc<u8c*>(s.data()), s.size())}; }

  u8c *data() const { return x.data(); }
  uN   size() const { return x.size(); }

  bool operator==(u9_symbol const &s) const { return h == s.h && x == s.x; }

  bool printable() const
    { for (let c : x) if (c < 32 || c >= 127) return false;
      return true; }
};


template<class T>
struct u9_struct { T x; };


template<class T> struct u9t_ { sletc t = u9t::none; };

template<class T> struct u9t_<T&>       { sletc t = u9t_<T>::t; };
template<class T> struct u9t_<T const>  { sletc t = u9t_<T>::t; };
template<class T> struct u9t_<T const&> { sletc t = u9t_<T>::t; };

template<> struct u9t_<u8>   { sletc t = u9t::u8;  };
template<> struct u9t_<u16>  { sletc t = u9t::u16; };
template<> struct u9t_<u32>  { sletc t = u9t::u32; };
template<> struct u9t_<u64>  { sletc t = u9t::u64; };

template<> struct u9t_<i8>   { sletc t = u9t::i8;  };
template<> struct u9t_<i16>  { sletc t = u9t::i16; };
template<> struct u9t_<i32>  { sletc t = u9t::i32; };
template<> struct u9t_<i64>  { sletc t = u9t::i64; };

template<> struct u9t_<f32>  { sletc t = u9t::f32; };
template<> struct u9t_<f64>  { sletc t = u9t::f64; };
template<> struct u9t_<c32>  { sletc t = u9t::c32; };
template<> struct u9t_<c64>  { sletc t = u9t::c64; };

template<> struct u9t_<bool>      { sletc t = u9t::b;      };
template<> struct u9t_<u9_symbol> { sletc t = u9t::symbol; };
template<> struct u9t_<u9_none>   { sletc t = u9t::none;   };
template<> struct u9t_<u9_stream> { sletc t = u9t::stream; };

template<> struct u9t_<B>    { sletc t = u9t::bytes; };
template<> struct u9t_<Bv>   { sletc t = u9t::bytes; };
template<> struct u9t_<St>   { sletc t = u9t::utf8; };
template<> struct u9t_<Stv>  { sletc t = u9t::utf8; };
template<> struct u9t_<chc*> { sletc t = u9t::utf8; };

template<class U>          struct u9t_<V<U>>    { sletc t = u9t::tuple; };
template<class K, class V> struct u9t_<M<K, V>> { sletc t = u9t::map; };
template<class U>          struct u9t_<S<U>>    { sletc t = u9t::set; };

template<class T> struct u9t_<u9_scoped<u9_π,     T>> { sletc t = u9t::pi; };
template<class T> struct u9t_<u9_scoped<u9_Φ,     T>> { sletc t = u9t::phi; };
template<class T> struct u9t_<u9_scoped<u9_host,  T>> { sletc t = u9t::host; };
template<class T> struct u9t_<u9_scoped<u9_build, T>> { sletc t = u9t::build; };


template<class T>
concept u9t_hastype = u9t_<T>::t != u9t::none;


// NOTE: some compilers define native-width ints in such a way that this
// is not trivially true -- i.e. although they are the same size, the
// template args fail to match.
static_assert(u9t_hastype<uN>);
static_assert(u9t_hastype<iN>);


enum class u9s
{
  v8  = 0,
  v16 = 1,
  v32 = 2,
  v64 = 3,
};


u8c constexpr u9f = 4;

ic uf8  operator|(u9t t, u9s s) { return Sc<uf8>(t) << 3 | Sc<uf8>(s); }
ic u9t  u9ts_t   (uf8 x)        { return Sc<u9t>(x >> 3); }
ic u9s  u9ts_s   (uf8 x)        { return Sc<u9s>(x  & 3); }
ic bool u9ts_f   (uf8 x)        { return Sc<bool>(x & 4); }

ic uN u9sb(u9s s)  // size of size+control bytes (i.e. prefix)
{
  switch (s)
  {
  case u9s::v8:  return 2;
  case u9s::v16: return 3;
  case u9s::v32: return 5;
  case u9s::v64: return 9;
    TA(0, "u9sb");
  }
}

ic u9s u9sq(uN s)
{
  return ou<u32>(s) ? u9s::v64
       : ou<u16>(s) ? u9s::v32
       : ou<u8>(s)  ? u9s::v16
       :              u9s::v8;
}


template<class T>
ic uN u9rs(T xs, uN i)
{
  switch (u9ts_s(R<u8>(xs, 0)))
  {
  case u9s::v8:  return 2 + R<u8> (xs, 1);
  case u9s::v16: return 3 + R<u16>(xs, 1);
  case u9s::v32: return 5 + R<u32>(xs, 1);
  case u9s::v64: return 9 + R<u64>(xs, 1);
    TA(0, "u9rs invalid size prefix: " << R<u8>(xs, 0))
  }
}


template<class T>
ic uN u9ws(T xs, uN i, u9t t, uN s, bool f = false)
{
  let q = u9sq(s);
  W(xs, i, t | q);
  if (f) xs[i] |= u9f;
  switch (q)
  {
  case u9s::v8:  W(xs, i + 1, Sc<u8>(s));  return 2;
  case u9s::v16: W(xs, i + 1, Sc<u16>(s)); return 3;
  case u9s::v32: W(xs, i + 1, Sc<u32>(s)); return 5;
  case u9s::v64: W(xs, i + 1, Sc<u64>(s)); return 9;
    TA(0, "u9ws invalid size prefix: " << Sc<uN>(q));
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


letc u9unsigned = u9tm{u9t::u8, u9t::u16, u9t::u32, u9t::u64};
letc u9signed   = u9tm{u9t::i8, u9t::i16, u9t::i32, u9t::i64};
letc u9ints     = u9unsigned | u9signed;
letc u9reals    = u9ints  | u9tm{u9t::f32, u9t::f64};
letc u9numbers  = u9reals | u9tm{u9t::c32, u9t::c64};

letc u9strings  = u9tm{u9t::bytes, u9t::utf8};
letc u9coll     = u9tm{u9t::index, u9t::map, u9t::set, u9t::tuple, u9t::tensor};

letc u9fixed    = u9numbers | u9tm{u9t::b, u9t::none};
letc u9atomics  = u9strings | u9numbers | u9tm{u9t::b, u9t::symbol};
letc u9vectors  = u9numbers;  // TODO: add bools


template<class T, u32 M>
struct u9t_is { sc let v = !!(M & 1 << Sc<u8>(u9t_<T>::t)); };

static_assert( u9t_is<u64, u9unsigned.m>::v);
static_assert(!u9t_is<i64, u9unsigned.m>::v);


ic uN u9logsizeof(u9t t)
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

ic uN u9sizeof(u9t t) { return 1 << u9logsizeof(t); }


#if τdebug_iostream
O &operator<<(O &s, u9t t)
{
  switch (t)
  {
  case u9t::i8:      return s << "i8";
  case u9t::i16:     return s << "i16";
  case u9t::i32:     return s << "i32";
  case u9t::i64:     return s << "i64";
  case u9t::u8:      return s << "u8";
  case u9t::u16:     return s << "u16";
  case u9t::u32:     return s << "u32";
  case u9t::u64:     return s << "u64";

  case u9t::f32:     return s << "f32";
  case u9t::f64:     return s << "f64";
  case u9t::c32:     return s << "c32";
  case u9t::c64:     return s << "c64";
  case u9t::b:       return s << "b";
  case u9t::symbol:  return s << "symbol";
  case u9t::stream:  return s << "stream";

  case u9t::bytes:   return s << "bytes";
  case u9t::utf8:    return s << "utf8";
  case u9t::index:   return s << "index";

  case u9t::tuple:   return s << "tuple";
  case u9t::map:     return s << "map";
  case u9t::set:     return s << "set";
  case u9t::tensor:  return s << "tensor";

  case u9t::phi:     return s << "Φ";
  case u9t::host:    return s << "host";
  case u9t::build:   return s << "build";

  case u9t::pi:      return s << "π";
  case u9t::none:    return s << "none";
  case u9t::frame:   return s << "frame";

    TA(s, Sc<uN>(t))
  }
}

O &operator<<(O &s, u9s x)
{
  switch (x)
  {
  case u9s::v8:  return s << "v8";
  case u9s::v16: return s << "v16";
  case u9s::v32: return s << "v32";
  case u9s::v64: return s << "v64";
    TA(s, Sc<uN>(x))
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

O &operator<<(O &s, u9st x)
{
  switch (x)
  {
  case u9st::ω: return s << "ω";
  case u9st::τ: return s << "τ";
  case u9st::θ: return s << "θ";
  case u9st::ι: return s << "ι";
  case u9st::κ: return s << "κ";
    TA(s, Sc<uN>(x))
  }
}

O &operator<<(O &s, u9_π const &p)
{
  switch (p)
  {
  case u9_π::ref: return s << "ref";
    TA(s, Sc<uN>(p));
  }
}

O &operator<<(O &s, u9_Φ const &f)
{
  switch (f)
  {
  case u9_Φ::fd: return s << "fd";
  case u9_Φ::heapref: return s << "heapref";
  case u9_Φ::heappin: return s << "heappin";
    TA(s, Sc<uN>(f));
  }
}

O &operator<<(O &s, u9_host const &h)
{
  switch (h)
  {
  case u9_host::path: return s << "path";
  case u9_host::pid:  return s << "pid";
    TA(s, Sc<uN>(h));
  }
}

O &operator<<(O &s, u9_build const &b)
{
  switch (b)
  {
  case u9_build::istruct: return s << "struct";
    TA(s, Sc<uN>(b));
  }
}

O &operator<<(O &s, u9_none const &n)
{
  switch (n)
  {
  case u9_none::generic:            return s << "∅[]";
  case u9_none::key_lookup_missing: return s << "∅[k→]";
  case u9_none::tuple_bounds_error: return s << "∅[<|≥]";
  }
}

O &operator<<(O &s, u9_symbol const &y)
{
  s << "'";
  if (y.printable()) return s << Stv(Rc<chc*>(y.x.data()), y.x.size());
  for (let c : y.x)
    s << "0123456789abcdef"[c >> 4]
      << "0123456789abcdef"[c & 15];
  return s;
}

template<class E>
O &operator<<(O &s, u9_scoped<E, u8*> const &x)
{
  return s << "u9s{" << x.t << ", " << Rc<void*>(x.x) << "}";
}
#endif


}


template<> struct std::hash<τ::u9_symbol>
{ inline τ::uN operator()(τ::u9_symbol const &s) const { return s.h; } };


#include "end.hh"


#endif
