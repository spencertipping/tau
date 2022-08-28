#ifndef tau_utf9_o9_h
#define tau_utf9_o9_h


#include <cstring>

#include "debug.hh"
#include "types.hh"
#include "zeta.hh"

#include "utf9-types.hh"

#include "begin.hh"

namespace tau
{


// TODO: add autoboxing for off-heap references
// TODO: o9 variant type


// TODO: we'll add to this once we have an o9 variant
template<class T> concept o9mapped = u9t_hastype<T>;

template<class T> concept o9fixed  = u9t_is<T, u9fixed.m>::v;
template<class T> concept o9string = u9t_is<T, u9strings.m>::v;
template<class T> concept o9coll   = u9t_is<T, u9coll.m>::v;


template<o9fixed T>
struct o9f
{
  T x;
  uN   size ()     const { return sizeof(T) + u9sb(u9sq(sizeof(T))); }
  void write(ζp m) const { W<T>(m, u9ws(m, 0, u9t_<T>::t, sizeof(T)), x); }
};


template<o9string T>
struct o9b
{
  T const &x;

  uN size() const { return x.size() + u9sb(u9sq(x.size())); }
  void write(ζp m) const
    { uN i = u9ws(m, 0, u9t_<T>::t, size());
      std::memcpy(m + i, x.data(), x.size()); }
};


struct o9c
{
  u9t const  t;
  chc       *xs;
  uNc        n;

  uN size() const { return n + u9sb(u9sq(n)); }
  void write(ζp m) const
    { uN i = u9ws(m, 0, t, n);
      std::memcpy(m + i, xs, n); }
};

inline o9c o9(chc *xs, uN n, u9t t = u9t::utf8) { return o9c{t, xs, n}; }
inline o9c o9(chc *xs)                          { return o9(xs, std::strlen(xs), u9t::utf8); }


template<o9mapped T, template<typename...> class C, class... Ts> struct o9v;
template<o9mapped K, o9mapped V, class... Ts>                    struct o9m;

template<o9fixed  T> ic o9f<T> o9(T x) { return o9f<T>{x}; }
template<o9string T> ic o9f<T> o9(T x) { return o9b<T>{x}; }

template<o9mapped T, class... Ts> ic o9v<T, V, Ts...> o9(V<T, Ts...> const&);
template<o9mapped T, class... Ts> ic o9v<T, S, Ts...> o9(S<T, Ts...> const&);

template<o9mapped K, o9mapped V, class... Ts>
ic o9m<K, V> o9(M<K, V, Ts...> const&);


template<o9fixed T>
struct o9a
{
  T const *xs;
  uNc      n;

  uN size()  const { return isize() + u9sb(u9sq(isize())); }
  uN isize() const { return u9sizeof(u9t_<T>::t) * n; }
  void write(ζp m) const
    { uN b = u9ws(m, 0, u9t_<T>::t, isize());
      for (uN i = 0; i < n; ++i)
        W(m + b, i * u9sizeof(u9t_<T>::t), xs[i]); }
};


template<o9mapped T, template<typename...> class C, class... Ts>
struct o9v  // unindexed, unordered tuple/set
{
  C<T, Ts...> const &xs;
  uN                 s = 0;

  uN size() { return isize() + u9sb(u9sq(isize())); }
  uN isize()
    { if (!s) for (let &x : xs) s += o9(x).size();
      return s; }

  void write(ζp m)
    { uN i = u9ws(m, 0, u9t::tuple, isize());
      for (let &x : xs) { auto o = o9(x); o.write(m + i); i += o.size(); } }
};


template<o9mapped K, o9mapped V, class... Ts>
struct o9m  // unindexed, unordered k/v map
{
  M<K, V, Ts...> const &xs;
  uN                    s = 0;

  uN size() { return isize() + u9sb(u9sq(isize())); }
  uN isize()
    { if (!s) for (let &[k, v] : xs) s += o9(k).size() + o9(v).size();
      return s; }

  void write(ζp m)
    { uN i = u9ws(m, 0, u9t::map, isize());
      for (let &[k, v] : xs)
      { let ok = o9(k); ok.write(m + i); i += ok.size();
        let ov = o9(v); ov.write(m + i); i += ov.size(); } }
};


template<o9mapped... X>
struct o9t
{
  T<X...> const xs;
  o9t(X... xs_) : xs(std::make_tuple(xs_...)) {}

  template<uN i = 0>
  typename std::enable_if<i == sizeof...(X), uN>::type
  isize() { return 0; }

  template<uN i = 0>
  typename std::enable_if<i < sizeof...(X), uN>::type
  isize() { return o9(std::get<i>(xs)).size() + isize<i + 1>(); }

  uN size()
    { let n = isize<0>();
      return n + u9sb(u9sq(n)); }

  template<uN i = 0>
  typename std::enable_if<i == sizeof...(X), void>::type
  write(ζp m) {}

  template<uN i = 0>
  typename std::enable_if<i < sizeof...(X), void>::type
  write(ζp m)
    { if (!i) m += u9ws(m, 0, u9t::tuple, isize<0>());
      let o = o9(std::get<i>(xs));
      o.write(m);
      write<i + 1>(m + o.size()); }
};


// TODO: generate indexes if these collections are very large
template<o9mapped T, class... Ts>
ic o9v<T, V, Ts...> o9(V<T, Ts...> const &xs) { return o9v<T, V, Ts...>{xs}; }

template<o9mapped T, class... Ts>
ic o9v<T, S, Ts...> o9(S<T, Ts...> const &xs) { return o9v<T, S, Ts...>{xs}; }

template<o9mapped K, o9mapped V, class... Ts>
ic o9m<K, V, Ts...> o9(M<K, V, Ts...> const &xs) { return o9m<K, V, Ts...>{xs}; }

template<o9fixed T>
ic o9a<T> o9(T const *xs, uN n) { return o9a<T>{xs, n}; }

template<o9fixed T>
ic o9a<T> o9(T const *b, T const *e) { return o9(b, e - b); }


}

#include "end.hh"


#endif
