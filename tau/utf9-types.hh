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
  enum class u9st;
  struct     u9tm;

  O &operator<<(O &s, u9t);
  O &operator<<(O &s, u9s);
  O &operator<<(O &s, u9st);
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
  heapref = 25,  // heapref that will be freed automatically
  heappin = 26,  // pinned heapref (not freed automatically)
  nstruct = 27,  // native-encoded struct

  none    = 31,  // not a real type
};


enum class u9st
{
  ω = 1,
  τ = 2,
  θ = 3,
  ι = 4,
  κ = 5,
};


static_assert(sizeof(pid_t) <= sizeof(u32));
static_assert(sizeof(int)   <= sizeof(u32));

struct u9_pidfd { u32 pid; u32 fd; };
struct u9_stream { u9st t; u64 n; };

defR(u9_pidfd) { return u9_pidfd{R<u32>(xs, i), R<u32>(xs, i + 4)}; }
defW(u9_pidfd) { W(xs, i, x.pid); W(xs, i + 4, x.fd); }


sletc u9ω = u9_stream{u9st::ω, 0};
sletc u9τ = u9_stream{u9st::τ, 0};
sletc u9κ = u9_stream{u9st::κ, 0};

ic u9_stream u9θ(u64 x) { return u9_stream{u9st::θ, x}; }
ic u9_stream u9θ(f64 x) { return u9_stream{u9st::θ, Sc<u64>(x * Sc<f64>(Nl<u64>::max()))}; }
ic u9_stream u9ι(u64 x) { return u9_stream{u9st::ι, x}; }


struct u9_heapref { void* r; };
defR(u9_heapref) { return u9_heapref{Rc<void*>(R<uN>(xs, i))}; }
defW(u9_heapref) { W<uN>(xs, i, Rc<uN>(x.r)); }


template<class T>
struct u9_struct { T x; };


template<class T> struct u9t_{ sletc t = u9t::none; };

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

template<> struct u9t_<bool> { sletc t = u9t::b; };
template<> struct u9t_<B>    { sletc t = u9t::bytes; };
template<> struct u9t_<Bv>   { sletc t = u9t::bytes; };
template<> struct u9t_<St>   { sletc t = u9t::utf8; };
template<> struct u9t_<Stv>  { sletc t = u9t::utf8; };
template<> struct u9t_<chc*> { sletc t = u9t::utf8; };

template<class U>          struct u9t_<V<U>>    { sletc t = u9t::tuple; };
template<class K, class V> struct u9t_<M<K, V>> { sletc t = u9t::map; };
template<class U>          struct u9t_<S<U>>    { sletc t = u9t::set; };

template<>        struct u9t_<u9_pidfd>     { sletc t = u9t::pidfd; };
template<>        struct u9t_<u9_stream>    { sletc t = u9t::stream; };
template<>        struct u9t_<u9_heapref>   { sletc t = u9t::heapref; };
template<class U> struct u9t_<u9_struct<U>> { sletc t = u9t::nstruct; };


template<class T>
concept u9t_hastype = u9t_<T>::t != u9t::none;


// NOTE: some compilers define native-width ints in such a way that this
// is not trivially true -- i.e. although they are the same size, the
// template args fail to match.
static_assert(u9t_hastype<uN>);
static_assert(u9t_hastype<iN>);


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


ic uf8 operator|(u9t t, u9s s) { return Sc<uf8>(t) << 3 | Sc<uf8>(s); }
ic u9t u9ts_t   (uf8 x)        { return Sc<u9t>(x >> 3); }
ic u9s u9ts_s   (uf8 x)        { return Sc<u9s>(x  & 7); }

ic bool u9sv(u9s s) { return Sc<uN>(s) >= 4; }

ic uN u9sb(u9s s)  // size of size+control bytes (i.e. prefix)
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

ic u9s u9sq(uN s)
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
ic uN u9rs(T xs, uN i)
{
  switch (u9ts_s(R<u8>(xs, 0)))
  {
  case u9s::f1:  return 1 + 1;
  case u9s::f2:  return 1 + 2;
  case u9s::f4:  return 1 + 4;
  case u9s::f8:  return 1 + 8;
  case u9s::v8:  return 2 + R<u8> (xs, 1);
  case u9s::v16: return 3 + R<u16>(xs, 1);
  case u9s::v32: return 5 + R<u32>(xs, 1);
  case u9s::v64: return 9 + R<u64>(xs, 1);
    TA(0, "control byte = " << R<u8>(xs, 0))
  }
}


template<class T>
ic uN u9ws(T xs, uN i, u9t t, uN s)
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


letc u9unsigned = u9tm{u9t::u8, u9t::u16, u9t::u32, u9t::u64};
letc u9signed   = u9tm{u9t::i8, u9t::i16, u9t::i32, u9t::i64};
letc u9ints     = u9unsigned | u9signed;
letc u9reals    = u9ints  | u9tm{u9t::f32, u9t::f64};
letc u9numbers  = u9reals | u9tm{u9t::c32, u9t::c64};

letc u9strings  = u9tm{u9t::bytes, u9t::utf8};
letc u9coll     = u9tm{u9t::index, u9t::map, u9t::set, u9t::tuple, u9t::tensor};
letc u9native   = u9tm{u9t::pidfd, u9t::heapref};

letc u9fixed    = u9numbers | u9tm{u9t::b} | u9native;
letc u9atomics  = u9strings | u9numbers | u9tm{u9t::b, u9t::symbol};
letc u9vectors  = u9numbers;  // TODO: add bools


template<class T, u32 M>
struct u9t_is { static constexpr bool const v = M & 1 << Sc<u8>(u9t_<T>::t); };

static_assert( u9t_is<u64, u9unsigned.m>::v);
static_assert(!u9t_is<i64, u9unsigned.m>::v);

static_assert(u9t_is<u9_pidfd, u9fixed.m>::v);


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


#if tau_debug_iostream
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

  case u9t::pidfd:   return s << "pidfd";
  case u9t::heapref: return s << "heapref";
  case u9t::heappin: return s << "heappin";
  case u9t::nstruct: return s << "struct";

    TA(s, Sc<uN>(t))
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
#endif


}

#include "end.hh"


#endif
