#ifndef τutf9_o9_h
#define τutf9_o9_h


#include <cstring>
#include <memory>

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>


#include "debug.hh"
#include "types.hh"
#include "zeta.hh"

#include "utf9-types.hh"
#include "utf9-i9.hh"

#include "begin.hh"

namespace τ
{


struct o9V  // o9 virtual base
{
  virtual ~o9V() {}
  virtual uN size ()     const = 0;
  virtual uN write(ζp m) const = 0;

  static ic uN osize(uN s) { return s + u9sb(u9sq(s)); }
};


template<class T> struct o9_      { sletc v = false; };
template<>        struct o9_<o9V> { sletc v = true; };

template<class T> concept O9  =  o9_<T>::v;
template<class T> concept O9N = !o9_<T>::v;

template<class T> concept o9fixed  = u9t_is<T, u9fixed.m>::v;
template<class T> concept o9string = u9t_is<T, u9strings.m>::v;
template<class T> concept o9coll   = u9t_is<T, u9coll.m>::v;

// NOTE: o9simple describes types that are low-effort to size and write.
// This matters for container o9 optimization, as dichotomized into
// o9v<> (simple) and o9vc<> (complex).
template<class T> concept o9mapped = O9<T> || u9t_hastype<T> || std::is_same_v<T, i9>;
template<class T> concept o9simple = o9mapped<T> && O9N<T> && !o9coll<T>;


template<O9 T> inline T o9(T x) { return x; }


struct o9i9 : virtual o9V
{
  // NOTE: the i9 may not be encoded with an optimal size, so copy verbatim
  // even if we could compact the size by a bit. The invariant here is that
  // output == input, down to the byte level.
  i9 const a;
  o9i9(i9 a_) : a(a_) {}
  uN size ()     const { return a.size() + u9sb(a.stype()); }
  uN write(ζp m) const { std::memcpy(m, a.a, size()); return 0; }
};

inline o9i9 o9(i9 i) { return o9i9{i}; }


struct o9q : virtual o9V  // byte-quoted i9 serialization
{
  i9 const x;
  o9q(i9 x_) : x(x_) {}
  uN size ()     const { return osize(x.osize()); }
  uN write(ζp m) const
    { let o = o9i9{x};
      let i = u9ws(m, 0, u9t::bytes, o.size());
      A(!o.write(m + i), "o9q internal error");
      return 0; }
};


template<class T>
struct o9f : virtual o9V
{
  sletc s = u9_sizeof<T>::v;
  T x;
  o9f(T x_) : x(x_) {}
  uN size ()     const { return osize(s); }
  uN write(ζp m) const { W(m, u9ws(m, 0, u9t_<T>::t, s), x); return 0; }
};

template<>
struct o9f<bool> : virtual o9V
{
  bool x;
  o9f(bool x_) : x(x_) {}
  uN size()      const { return osize(1); }
  uN write(ζp m) const { W(m, u9ws(m, 0, u9t::b, 1), Sc<u8>(x ? 17 : 1)); return 0; }
};


template<class T>
o9f<u9_scoped<u9_Φ, ζp>> o9box(T &x)
{
  let r = Sc<ζp>(std::malloc(x.size()));
  x.write(r);
  return o9f{u9_scoped<u9_Φ, ζp>{u9_Φ::heapref, r}};
}


template<class T>
struct o9b : virtual o9V
{
  T x;
  o9b(T x_) : x(x_) {}
  uN size() const { return osize(x.size()); }
  uN write(ζp m) const
    { uN i = u9ws(m, 0, u9t_<T>::t, x.size());
      std::memcpy(m + i, x.data(), x.size());
      return 0; }
};


struct o9st : virtual o9V
{
  u9_stream s;
  o9st(u9_stream const &s_) : s(s_) {}

  uN isize() const
    { switch (s.t)
      {
      case u9st::α:
      case u9st::ω:
      case u9st::τ:
      case u9st::κ: return 2;
      case u9st::θ:
      case u9st::ι: return 2 + sizeof(s.n);
        TA(0, Sc<uN>(s.t))
      } }

  uN size() const { return osize(isize()); }
  uN write(ζp m) const
    { uN i = u9ws(m, 0, u9t::stream, isize());
      W(m, i, Sc<u8>(s.t));
      W(m, i + 1, s.d);

      if (s.t == u9st::θ || s.t == u9st::ι) W(m, i + 2, s.n);
      return 0; }
};

inline o9st o9(u9_stream s) { return o9st{s}; }


struct o9c : virtual o9V
{
  u9t const  t;
  chc       *xs;
  uNc        n;

  o9c(u9t const t_, chc *xs_, uN n_) : t(t_), xs(xs_), n(n_) {}

  uN size() const { return osize(n); }
  uN write(ζp m) const
    { uN i = u9ws(m, 0, t, n);
      std::memcpy(m + i, xs, n);
      return 0; }
};

inline o9c o9(chc *xs, uN n, u9t t = u9t::utf8) { return o9c{t, xs, n}; }
inline o9c o9(chc *xs)                          { return o9(xs, std::strlen(xs), u9t::utf8); }


template<o9simple T, template<typename...> class C, class... Ts> struct o9v;
template<o9mapped K, o9simple T, class... Ts>                    struct o9m;
template<o9coll   T, template<typename...> class C, class... Ts> struct o9vc;
template<o9mapped K, o9coll   T, class... Ts>                    struct o9mc;

template<o9fixed T>        inline o9f<T>               o9(T x)               { return o9f<T>{x}; }
template<class E, class T> inline o9f<u9_scoped<E, T>> o9(u9_scoped<E, T> x) { return o9f<u9_scoped<E, T>>{x}; }

inline o9b<B const&>         o9(B         const &x) { return o9b<B const&>        {x}; }
inline o9b<Bv>               o9(Bv               x) { return o9b<Bv>              {x}; }
inline o9b<St const&>        o9(St        const &x) { return o9b<St const&>       {x}; }
inline o9b<Stv>              o9(Stv              x) { return o9b<Stv>             {x}; }
inline o9b<u9_symbol const&> o9(u9_symbol const &s) { return o9b<u9_symbol const&>{s}; }

template<o9simple T, class... Ts> inline o9v<T, V, Ts...> o9(V<T, Ts...> const&);
template<o9simple T, class... Ts> inline o9v<T, S, Ts...> o9(S<T, Ts...> const&);

template<o9mapped K, o9simple T, class... Ts>
inline o9m<K, T, Ts...> o9(M<K, T, Ts...> const&);

template<o9coll T, class... Ts> inline o9vc<T, V, Ts...> o9(V<T, Ts...> const&);
template<o9coll T, class... Ts> inline o9vc<T, S, Ts...> o9(S<T, Ts...> const&);

template<o9mapped K, o9coll T, class... Ts>
inline o9mc<K, T, Ts...> o9(M<K, T, Ts...> const&);


template<o9fixed T>
struct o9a : virtual o9V  // vector of fixed
{
  T const *xs;
  uNc      n;

  o9a(T const *xs_, uN n_) : xs(xs_), n(n_) {}

  uN size()  const { return osize(isize()); }
  uN isize() const { return u9sizeof(u9t_<T>::t) * n; }
  uN write(ζp m) const
    { uN b = u9ws(m, 0, u9t_<T>::t, isize());
      A(sizeof(T) == u9sizeof(u9t_<T>::t),
        "o9a internal error: " << sizeof(T) << " ≠ " << u9sizeof(u9t_<T>::t) << ", t = " << u9t_<T>::t);
      for (uN i = 0; i < n; ++i)
        W(m + b, i * u9sizeof(u9t_<T>::t), xs[i]);
      return 0; }
};


template<o9simple T, template<typename...> class C, class... Ts>
struct o9v : virtual o9V  // unindexed, unordered tuple/set (simple)
{
  C<T, Ts...> const &xs;
  uN        mutable  s = 0;

  o9v(C<T, Ts...> const &xs_) : xs(xs_) {}

  uN size()  const { return osize(isize()); }
  uN isize() const
    { if (!s) for (let &x : xs) s += o9(x).size();
      return s; }

  uN write(ζp m) const
  // FIXME: choose correct container type
    { uN   i = u9ws(m, 0, u9t::tuple, isize());
      bool f = false;
      for (let &x : xs)
      { auto o = o9(x);
        A(!o.write(m + i), "o9v element attempted to resize");
        f = f || u9ts_f(R<u8>(m, i));
        i += o.size(); }
      if (f) m[0] |= u9f;
      return 0; }
};


template<o9coll T, template<typename...> class C, class... Ts>
struct o9vc : virtual o9V  // unindexed, unordered tuple/set (complex)
{
  typedef decltype(o9(std::declval<T>())) T9;

  V<T9>      ys;
  uN mutable s = 0;

  o9vc(C<T, Ts...> const &xs)
    { ys.reserve(xs.size());
      for (let &x : xs) ys.push_back(o9(x)); }

  uN size()  const { return osize(isize()); }
  uN isize() const
    { if (!s) for (let &y : ys) s += y.size();
      return s; }

  uN write(ζp m) const
  // FIXME: should choose correct container type here
    { uN   i = u9ws(m, 0, u9t::tuple, isize());
      bool f = false;
      for (let &y : ys)
      { A(!y.write(m + i), "o9v resize");
        f = f || u9ts_f(R<u8>(m, i));
        i += y.size(); }
      if (f) m[0] |= u9f;
      return 0; }
};


template<o9mapped K, o9simple V, class... Ts>
struct o9m : virtual o9V  // unindexed, unordered k/v map
{
  M<K, V, Ts...> const &xs;
  uN mutable            s = 0;

  o9m(M<K, V, Ts...> const &xs_) : xs(xs_) {}

  uN size()  const { return osize(isize()); }
  uN isize() const
    { if (!s) for (let &[k, v] : xs) s += o9(k).size() + o9(v).size();
      return s; }

  uN write(ζp m) const
    { uN   i = u9ws(m, 0, u9t::map, isize());
      bool f = false;
      for (let &[k, v] : xs)
      { let ok = o9(k); ok.write(m + i); f = f || u9ts_f(R<u8>(m, i)); i += ok.size();
        let ov = o9(v); ov.write(m + i); f = f || u9ts_f(R<u8>(m, i)); i += ov.size(); }
      if (f) m[0] |= u9f;
      return 0; }
};


template<o9mapped K, o9coll T, class... Ts>
struct o9mc : virtual o9V  // unindexed, unordered k/v map
{
  typedef decltype(o9(std::declval<K>())) K9;
  typedef decltype(o9(std::declval<T>())) T9;

  V<K9>      ks;
  V<T9>      vs;
  uN mutable s = 0;

  o9mc(M<K, T, Ts...> const &m)
    { ks.reserve(m.size());
      vs.reserve(m.size());
      for (let &[k, v] : m) ks.push_back(o9(k)), vs.push_back(o9(v)); }

  uN size() const { return osize(isize()); }
  uN isize() const
    { if (!s)
      { for (let &k : ks) s += k.size();
        for (let &v : vs) s += v.size(); }
      return s; }

  uN write(ζp m) const
    { uN   i = u9ws(m, 0, u9t::map, isize());
      bool f = false;
      for (uN j = 0; j < ks.size(); ++j)
      { A(!ks[i].write(m + i), "o9m k resize"); f = f || u9ts_f(R<u8>(m, i)); i += ks[i].size();
        A(!vs[i].write(m + i), "o9m v resize"); f = f || u9ts_f(R<u8>(m, i)); i += vs[i].size(); }
      if (f) m[0] |= u9f;
      return 0; }
};


template<o9mapped... X>
struct o9t : virtual o9V
{
  // NOTE: hetereogeneous container can't easily cache o9s -- although
  // we could template-magic our way to doing that.

  // TODO: template magic to cache o9s

  u9t           t;
  T<X...> const xs;
  o9t(X... xs_) : t(u9t::tuple), xs(std::make_tuple(xs_...)) {}

  template<uN i = 0>
  typename std::enable_if<i == sizeof...(X), uN>::type
  isize() const { return 0; }

  template<uN i = 0>
  typename std::enable_if<i < sizeof...(X), uN>::type
  isize() const { return o9(std::get<i>(xs)).size() + isize<i + 1>(); }

  uN size() const { return osize(isize<0>()); }

  template<uN i>
  typename std::enable_if<!sizeof...(X), bool>::type
  write_(ζp m, bool f) const
    { u9ws(m, 0, t, isize<0>());
      return false; }

  template<uN i = 0>
  typename std::enable_if<sizeof...(X) && i == sizeof...(X), bool>::type
  write_(ζp m, bool f) const { return f; }

  template<uN i = 0>
  typename std::enable_if<sizeof...(X) && i < sizeof...(X), bool>::type
  write_(ζp m, bool f) const
    { if (!i) m += u9ws(m, 0, t, isize<0>());
      auto o = o9(std::get<i>(xs));
      A(!o.write(m), "o9t internal error");
      return write_<i + 1>(m + o.size(), f || u9ts_f(R<u8>(m, 0))); }

  uN write(ζp m) const
    { if (write_<0>(m, false)) m[0] |= u9f;
      return 0; }

  o9t &m() { t = u9t::map; return *this; }
  o9t &s() { t = u9t::set; return *this; }
};


template<o9fixed T>
struct o9vec : virtual o9V  // cleared vector of given size
{
  uNc n;
  o9vec(uN n_) : n(n_) {}
  uN isize()     const { return sizeof(T) * n; }
  uN size()      const { return osize(isize()); }
  uN write(ζp m) const
    { let i = u9ws(m, 0, u9t_<T>::t, isize());
      std::memset(m + i, 0, isize());
      return 0; }
};

template<>
struct o9vec<bool> : virtual o9V
{
  uNc n;
  o9vec(uN n_) : n(n_) {}
  uN isize()     const { return 1 + (n + 3 >> 3); }
  uN size()      const { return osize(isize()); }
  uN write(ζp m) const
    { let i = u9ws(m, 0, u9t::b, isize());
      std::memset(m + i, 0, isize());
      W(m, i, Sc<u8>(n & 7));
      return 0; }
};

struct o9vecsized : virtual o9V
{
  uNc       n;
  u9t const t;
  o9vecsized(uN n_, u9t t_) : n(n_), t(t_) {}
  uN size() const { return osize(n); }
  uN write(ζp m) const
    { let i = u9ws(m, 0, t, n);
      std::memset(m + i, 0, n);
      return 0; }
};


struct o9idx : virtual o9V
{
  i9  const x;
  uNc       n;
  u8c       b;
  u9t const t;

  o9idx(i9 x_, u8 b_)
    : x(x_), n(x.deref().len() >> b_), b(b_), t(u9tqu(x.osize()))
    { u9tm{u9t::tuple, u9t::set, u9t::map}(x.deref().type()); }

  uN isize() const { return osize(n * 2 * u9sizeof(t)) + x.osize() + o9(b).size(); }
  uN size()  const { return osize(isize()); }

  template<class U>
  void widx_t(i9 m) const
    { let a = ~(Sc<U>(-1) << b);
      let c = x.deref();
      U   i = 0;
      uN  j = 0;
      for (let y : c)
      { if (!(i & a) && j + 1 < m.vn())
        { m.set<U>(j++, i);
          m.set<U>(j++, Sc<U>(y.a - c.a)); }
        ++i; } }

  template<class U>
  void widx_s(i9 m) const
    { u64 h = 0;
      let c = x.deref();
      for (let y : c)
      { let yh = y.h();
        A(yh >= h, "widx_s h nonascending: " << yh << " < " << h);
        h = yh; }
      let a = ~(Sc<uN>(-1) << b);
      uN  i = 0;
      uN  j = 0;
      for (let y : c)
      { if (!(i & a) && j + 1 < m.vn())
        { m.set<U>(j++, y.h() >> (sizeof(u64) - sizeof(U)) * 8);
          m.set<U>(j++, Sc<U>(y.a - c.a)); }
        ++i; } }

  template<class U>
  void widx_m(i9 m) const
    { u64 h = 0;
      let c = x.deref();
      for (let y : c.keys())
      { let yh = y.h();
        A(yh >= h, "widx_m h nonascending: " << yh << " < " << h);
        h = yh; }
      let a = ~(Sc<uN>(-1) << b - 1);
      uN  i = 0;
      uN  j = 0;
      for (let y : c.keys())
      { if (!(i & a) && j + 1 < m.vn())
        { m.set<U>(j++, y.h() >> (sizeof(u64) - sizeof(U)) * 8);
          m.set<U>(j++, Sc<U>(y.a - c.a)); }
        ++i; } }

  template<class U>
  void widx(i9 m) const
    { switch (x.type())
      {
      case u9t::tuple: widx_t<U>(m); break;
      case u9t::set:   widx_s<U>(m); break;
      case u9t::map:   widx_m<U>(m); break;
        TA(, "o9idx invalid ctype: " << x.type());
      } }

  uN write(ζp m) const
    { let i = u9ws(m, 0, u9t::index, isize(), x.flagged());
      let v = i9{m + i};
      o9vecsized{n * 2 * u9sizeof(t), t}.write(m + i);
      A(!o9i9{x}.write(m + i + v.osize()), "o9idx resizing indexed object");
      switch (t)
      {
      case u9t::u8:  widx<u8> (v);  break;
      case u9t::u16: widx<u16>(v); break;
      case u9t::u32: widx<u32>(v); break;
      case u9t::u64: widx<u64>(v); break;
        TA(0, "o9idx::write u9tqu OOB: " << t);
      }
      let c = i9{m + i + v.osize()};
      o9(b).write(m + i + v.osize() + c.osize());
      return 0; }
};


template<o9simple T,             class... Ts> inline o9v<T, V, Ts...> o9(V<T,    Ts...> const &xs) { return o9v<T, V, Ts...>{xs}; }
template<o9simple T,             class... Ts> inline o9v<T, S, Ts...> o9(S<T,    Ts...> const &xs) { return o9v<T, S, Ts...>{xs}; }
template<o9mapped K, o9simple T, class... Ts> inline o9m<K, T, Ts...> o9(M<K, T, Ts...> const &xs) { return o9m<K, T, Ts...>{xs}; }

template<o9coll T,               class... Ts> inline o9vc<T, V, Ts...> o9(V<T,    Ts...> const &xs) { return o9vc<T, V, Ts...>{xs}; }
template<o9coll T,               class... Ts> inline o9vc<T, S, Ts...> o9(S<T,    Ts...> const &xs) { return o9vc<T, S, Ts...>{xs}; }
template<o9mapped K, o9coll T,   class... Ts> inline o9mc<K, T, Ts...> o9(M<K, T, Ts...> const &xs) { return o9mc<K, T, Ts...>{xs}; }

template<o9fixed T> inline o9a<T> o9(T const *xs, uN       n) { return o9a<T>{xs, n}; }
template<o9fixed T> inline o9a<T> o9(T const *b,  T const *e) { return o9(b, e - b); }


template<>           struct o9_<o9i9>       { sletc v = true; };
template<>           struct o9_<o9q>        { sletc v = true; };
template<class T>    struct o9_<o9f<T>>     { sletc v = true; };
template<class T>    struct o9_<o9b<T>>     { sletc v = true; };
template<>           struct o9_<o9st>       { sletc v = true; };
template<>           struct o9_<o9c>        { sletc v = true; };
template<class T>    struct o9_<o9a<T>>     { sletc v = true; };
template<class... T> struct o9_<o9m<T...>>  { sletc v = true; };
template<class... T> struct o9_<o9mc<T...>> { sletc v = true; };
template<class... T> struct o9_<o9t<T...>>  { sletc v = true; };
template<class T>    struct o9_<o9vec<T>>   { sletc v = true; };

template<>           struct o9_<o9idx>      { sletc v = true; };


// This one is special due to higher-order second type arg
template<o9mapped T, template<typename...> class C, class... X>
struct o9_<o9v<T, C, X...>> { sletc v = true; };

template<o9mapped T, template<typename...> class C, class... X>
struct o9_<o9vc<T, C, X...>> { sletc v = true; };

// These alternatives included for clang-based compilers
template<class... T> struct o9_<o9v<T...>>  { sletc v = true; };
template<class... T> struct o9_<o9vc<T...>> { sletc v = true; };


// Virtual delegation
struct o9vd : virtual o9V
{
  o9V const *const x;
  o9vd(o9V const *x_) : x(x_) {}
  uN size()      const { return x->size(); }
  uN write(ζp m) const { return x->write(m); }
};

template<> struct o9_<o9vd> { sletc v = true; };

inline o9vd o9(o9V const *x) { return o9vd(x); }


template<O9 T>
struct o9flag : virtual o9V
{
  T x;
  o9flag(T x_) : x(x_) {}
  uN size ()     const { return x.size(); }
  uN write(ζp m) const
    { let r = x.write(m);
      *m |= u9f;
      return r; }
};

template<O9 T> struct o9_<o9flag<T>> { sletc v = true; };


template<class T> auto o9ptr(T* x)
{ return o9(u9_scoped<u9_Φ, T*>{u9_Φ::ptr, x}); }


template<O9 T>
uN operator<<(B &i, T const &x)
{
  let s = x.size();
  let c = i.size();
  i.resize(c + s);
  if (let n = x.write(i.data() + c))
  { A(n != ζω, "B<< failed");
    i.resize(i.size() - s + n); }
  return c;
}


}

#include "end.hh"


#endif
