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
};


template<class T> concept o9mapped = u9t_hastype<T> || std::is_same_v<T, i9>;

template<class T> concept o9fixed  = u9t_is<T, u9fixed.m>::v;
template<class T> concept o9string = u9t_is<T, u9strings.m>::v;
template<class T> concept o9coll   = u9t_is<T, u9coll.m>::v;


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
  uN size ()     const { let s = i9::size_of(x.a); return s + u9sb(u9sq(s)); }
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
  uN size ()     const { return s + u9sb(u9sq(s)); }
  uN write(ζp m) const { W(m, u9ws(m, 0, u9t_<T>::t, s), x); return 0; }
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
  uN size() const { return x.size() + u9sb(u9sq(x.size())); }
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
      case u9st::ω: return 1;
      case u9st::τ: return 1;
      case u9st::θ: return 1 + sizeof(s.n);
        TA(0, Sc<uN>(s.t))
      } }

  uN size() const { return isize() + u9sb(u9sq(isize())); }
  uN write(ζp m) const
    { uN i = u9ws(m, 0, u9t::stream, isize());
      W(m, i, Sc<u8>(s.t));
      if (s.t == u9st::θ) W(m, i + 1, s.n);
      return 0; }
};

inline o9st o9(u9_stream s) { return o9st{s}; }


struct o9c : virtual o9V
{
  u9t const  t;
  chc       *xs;
  uNc        n;

  o9c(u9t const t_, chc *xs_, uN n_) : t(t_), xs(xs_), n(n_) {}

  uN size() const { return n + u9sb(u9sq(n)); }
  uN write(ζp m) const
    { uN i = u9ws(m, 0, t, n);
      std::memcpy(m + i, xs, n);
      return 0; }
};

inline o9c o9(chc *xs, uN n, u9t t = u9t::utf8) { return o9c{t, xs, n}; }
inline o9c o9(chc *xs)                          { return o9(xs, std::strlen(xs), u9t::utf8); }


template<o9mapped T, template<typename...> class C, class... Ts> struct o9v;
template<o9mapped K, o9mapped V, class... Ts>                    struct o9m;

template<o9fixed T>        inline o9f<T>               o9(T x)               { return o9f<T>{x}; }
template<class E, class T> inline o9f<u9_scoped<E, T>> o9(u9_scoped<E, T> x) { return o9f<u9_scoped<E, T>>{x}; }

inline o9b<B const&>         o9(B         const &x) { return o9b<B const&>        {x}; }
inline o9b<Bv>               o9(Bv               x) { return o9b<Bv>              {x}; }
inline o9b<St const&>        o9(St        const &x) { return o9b<St const&>       {x}; }
inline o9b<Stv>              o9(Stv              x) { return o9b<Stv>             {x}; }
inline o9b<u9_symbol const&> o9(u9_symbol const &s) { return o9b<u9_symbol const&>{s}; }

template<o9mapped T, class... Ts> inline o9v<T, V, Ts...> o9(V<T, Ts...> const&);
template<o9mapped T, class... Ts> inline o9v<T, S, Ts...> o9(S<T, Ts...> const&);

template<o9mapped K, o9mapped V, class... Ts>
inline o9m<K, V> o9(M<K, V, Ts...> const&);


template<o9fixed T>
struct o9a : virtual o9V  // vector of fixed
{
  T const *xs;
  uNc      n;

  o9a(T const *xs_, uN n_) : xs(xs_), n(n_) {}

  uN size()  const { return isize() + u9sb(u9sq(isize())); }
  uN isize() const { return u9sizeof(u9t_<T>::t) * n; }
  uN write(ζp m) const
    { uN b = u9ws(m, 0, u9t_<T>::t, isize());
      A(sizeof(T) == u9sizeof(u9t_<T>::t),
        "o9a internal error: " << sizeof(T) << " ≠ " << u9sizeof(u9t_<T>::t) << ", t = " << u9t_<T>::t);
      for (uN i = 0; i < n; ++i)
        W(m + b, i * u9sizeof(u9t_<T>::t), xs[i]);
      return 0; }
};


template<o9mapped T, template<typename...> class C, class... Ts>
struct o9v : virtual o9V  // unindexed, unordered tuple/set
{
  C<T, Ts...> const &xs;
  uN        mutable  s = 0;

  o9v(C<T, Ts...> const &xs_) : xs(xs_) {}

  uN size() const { return isize() + u9sb(u9sq(isize())); }
  uN isize() const
    { if (!s) for (let &x : xs) s += o9(x).size();
      return s; }

  uN write(ζp m) const
    { uN   i = u9ws(m, 0, u9t::tuple, isize());
      bool f = false;
      for (let &x : xs)
      { auto o = o9(x);
        A(!o.write(m + i), "o9v internal error");
        f = f || u9ts_f(R<u8>(m, i));
        i += o.size(); }
      if (f) m[i] |= u9f;
      return 0; }
};


template<o9mapped K, o9mapped V, class... Ts>
struct o9m : virtual o9V  // unindexed, unordered k/v map
{
  M<K, V, Ts...> const &xs;
  uN mutable            s = 0;

  o9m(M<K, V, Ts...> const &xs_) : xs(xs_) {}

  uN size() const { return isize() + u9sb(u9sq(isize())); }
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


template<o9mapped... X>
struct o9t : virtual o9V
{
  T<X...> const xs;
  o9t(X... xs_) : xs(std::make_tuple(xs_...)) {}

  template<uN i = 0>
  typename std::enable_if<i == sizeof...(X), uN>::type
  isize() const { return 0; }

  template<uN i = 0>
  typename std::enable_if<i < sizeof...(X), uN>::type
  isize() const { return o9(std::get<i>(xs)).size() + isize<i + 1>(); }

  uN size() const
    { let n = isize<0>();
      return n + u9sb(u9sq(n)); }

  template<uN i = 0>
  typename std::enable_if<i == sizeof...(X), bool>::type
  write_(ζp m, bool f) const { return f; }

  template<uN i = 0>
  typename std::enable_if<i < sizeof...(X), bool>::type
  write_(ζp m, bool f) const
    { if (!i) m += u9ws(m, 0, u9t::tuple, isize<0>());
      auto o = o9(std::get<i>(xs));
      A(!o.write(m), "o9t internal error");
      return write_<i + 1>(m + o.size(), f || u9ts_f(R<u8>(m, 0))); }

  uN write(ζp m) const
    { if (write_<0>(m, false)) m[0] |= u9f;
      return 0; }
};


template<o9fixed T>
struct o9vec : virtual o9V
{
  uN n;
  o9vec(uN n_) : n(n_) {}
  uN isize()     const { return sizeof(T) * n; }
  uN size()      const { return isize() + u9sb(u9sq(isize())); }
  uN write(ζp m) const
    { let i = u9ws(m, 0, u9t_<T>::t, isize());
      std::memset(m + i, 0, isize());
      return 0; }
};

template<>
struct o9vec<bool> : virtual o9V
{
  uN n;
  o9vec(uN n_) : n(n_) {}
  uN isize()     const { return n + 7 >> 3; }
  uN size()      const { return isize() + u9sb(u9sq(isize())); }
  uN write(ζp m) const
    { let i = u9ws(m, 0, u9t::b, isize());
      std::memset(m + i, 0, isize());
      return 0; }
};


template<o9mapped T, class... Ts>
inline o9v<T, V, Ts...> o9(V<T, Ts...> const &xs) { return o9v<T, V, Ts...>{xs}; }

template<o9mapped T, class... Ts>
inline o9v<T, S, Ts...> o9(S<T, Ts...> const &xs) { return o9v<T, S, Ts...>{xs}; }

template<o9mapped K, o9mapped V, class... Ts>
inline o9m<K, V, Ts...> o9(M<K, V, Ts...> const &xs) { return o9m<K, V, Ts...>{xs}; }

template<o9fixed T>
inline o9a<T> o9(T const *xs, uN n) { return o9a<T>{xs, n}; }

template<o9fixed T>
inline o9a<T> o9(T const *b, T const *e) { return o9(b, e - b); }


template<class T>    struct o9_            { sletc v = false; };
template<>           struct o9_<o9i9>      { sletc v =  true; };
template<>           struct o9_<o9q>       { sletc v =  true; };
template<class T>    struct o9_<o9f<T>>    { sletc v =  true; };
template<class T>    struct o9_<o9b<T>>    { sletc v =  true; };
template<>           struct o9_<o9st>      { sletc v =  true; };
template<>           struct o9_<o9c>       { sletc v =  true; };
template<class T>    struct o9_<o9a<T>>    { sletc v =  true; };
template<class... T> struct o9_<o9m<T...>> { sletc v =  true; };
template<class... T> struct o9_<o9t<T...>> { sletc v =  true; };
template<class T>    struct o9_<o9vec<T>>  { sletc v =  true; };

// This one is special due to higher-order second type arg
template<o9mapped T, template<typename> class C, class... X>
struct o9_<o9v<T, C, X...>> { sletc v = true; };


// Virtual delegation
struct o9vd : virtual o9V
{
  o9V const *x;
  o9vd(o9V const *x_) : x(x_) {}
  uN size()      const { return x->size(); }
  uN write(ζp m) const { return x->write(m); }
};

template<> struct o9_<o9vd> { sletc v = true; };

inline o9vd o9(o9V const *x) { return o9vd(x); }


template<class T> concept O9  =  o9_<T>::v;
template<class T> concept O9N = !o9_<T>::v;


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
