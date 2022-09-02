#ifndef tau_utf9_o9_h
#define tau_utf9_o9_h


#include <cstring>
#include <memory>

#include <errno.h>
#include <unistd.h>


#include "debug.hh"
#include "types.hh"
#include "zeta.hh"

#include "utf9-types.hh"
#include "utf9-i9.hh"

#include "begin.hh"

namespace tau
{


template<class T> concept o9mapped = u9t_hastype<T> || std::is_same_v<T, i9>;

template<class T> concept o9fixed  = u9t_is<T, u9fixed.m>::v;
template<class T> concept o9string = u9t_is<T, u9strings.m>::v;
template<class T> concept o9coll   = u9t_is<T, u9coll.m>::v;


struct o9i9
{
  i9 const a;
  uN size ()     const { let s = a.size(); return s + u9sb(a.stype()); }
  uN write(ζp m) const { std::memcpy(m, a.a, size()); return 0; }
};

inline o9i9 o9(i9 i) { return o9i9{i}; }


template<o9fixed T>
struct o9f
{
  T x;
  uN size ()     const { return sizeof(T) + u9sb(u9sq(sizeof(T))); }
  uN write(ζp m) const { W<T>(m, u9ws(m, 0, u9t_<T>::t, sizeof(T)), x); return 0; }
};


template<class T>
o9f<u9_heapref> o9box(T &x)
{
  let r = std::malloc(x.size());
  x.write(Sc<ζp>(r));
  return o9f<u9_heapref>{u9_heapref{r}};
}


template<o9string T>
struct o9b
{
  T const &x;

  uN size() const { return x.size() + u9sb(u9sq(x.size())); }
  uN write(ζp m) const
    { uN i = u9ws(m, 0, u9t_<T>::t, size());
      std::memcpy(m + i, x.data(), x.size());
      return 0; }
};


struct o9st
{
  u9_stream s;

  uN isize() const
    { switch (s.t)
      {
      case u9st::ω: return 1;
      case u9st::τ: return 1;
      case u9st::θ: return 9;
        TA(0, Sc<uN>(s.t))
      } }

  uN size() const { return isize() + u9sb(u9sq(isize())); }
  uN write(ζp m) const
    { uN i = u9ws(m, 0, u9t::stream, isize());
      W(m, i, Sc<u8>(s.t));
      if (s.t == u9st::θ) W<u64>(m, i + 1, s.n);
      return 0; }
};

ic o9st o9(u9_stream s) { return o9st{s}; }


struct o9c
{
  u9t const  t;
  chc       *xs;
  uNc        n;

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

template<o9fixed  T> ic o9f<T> o9(T x) { return o9f<T>{x}; }
template<o9string T> ic o9f<T> o9(T x) { return o9b<T>{x}; }

template<o9mapped T, class... Ts> ic o9v<T, V, Ts...> o9(V<T, Ts...> const&);
template<o9mapped T, class... Ts> ic o9v<T, S, Ts...> o9(S<T, Ts...> const&);

template<o9mapped K, o9mapped V, class... Ts>
ic o9m<K, V> o9(M<K, V, Ts...> const&);


template<o9fixed T>
struct o9a  // vector of fixed
{
  T const *xs;
  uNc      n;

  uN size()  const { return isize() + u9sb(u9sq(isize())); }
  uN isize() const { return u9sizeof(u9t_<T>::t) * n; }
  uN write(ζp m) const
    { uN b = u9ws(m, 0, u9t_<T>::t, isize());
      for (uN i = 0; i < n; ++i)
        W(m + b, i * u9sizeof(u9t_<T>::t), xs[i]);
      return 0; }
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

  uN write(ζp m)
    { uN i = u9ws(m, 0, u9t::tuple, isize());
      for (let &x : xs) { auto o = o9(x); o.write(m + i); i += o.size(); }
      return 0; }
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

  uN write(ζp m)
    { uN i = u9ws(m, 0, u9t::map, isize());
      for (let &[k, v] : xs)
      { let ok = o9(k); ok.write(m + i); i += ok.size();
        let ov = o9(v); ov.write(m + i); i += ov.size(); }
      return 0; }
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
  typename std::enable_if<i == sizeof...(X), uN>::type
  write(ζp m) { return 0; }

  template<uN i = 0>
  typename std::enable_if<i < sizeof...(X), uN>::type
  write(ζp m)
    { if (!i) m += u9ws(m, 0, u9t::tuple, isize<0>());
      auto o = o9(std::get<i>(xs));
      o.write(m);
      write<i + 1>(m + o.size());
      return 0; }
};


struct o9fdr  // zero-copy read from FD
{
  sletc sb = u9sb(u9s::v32);  static_assert(sb == 5);
  uNc   fd;
  iN   &n;    // NOTE: must be external to this o9, since o9 objects
  iN   &e;    // are assumed to be trivially copyable/destructible
  u32c  s;

  o9fdr(uN fd_, iN &n_, iN &e_, u32 s_ = 1 << ζb0 - 1)
    : fd(fd_), n(n_), e(e_), s(s_ - sb)
    { A(s_ > sb, "o9fdr " << s_ << "(s_) ≤ " << sb << "(sb)"); }

  uN size() { return s + sb; }
  uN write(ζp m)
    { n = read(fd, m + sb, s);
      if (n <= 0) { if (n == -1) e = errno; return ζω; }
      e = 0;
      W<u8>(m, 0, u9t::bytes | u9s::v32);
      W<u32>(m, 1, n);
      return n + sb; }
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


template<class T>    struct o9_            { sletc v = false; };
template<>           struct o9_<o9i9>      { sletc v = true; };
template<>           struct o9_<o9fdr>     { sletc v = true; };
template<class T>    struct o9_<o9f<T>>    { sletc v = true; };
template<class T>    struct o9_<o9b<T>>    { sletc v = true; };
template<>           struct o9_<o9st>      { sletc v = true; };
template<>           struct o9_<o9c>       { sletc v = true; };
template<class T>    struct o9_<o9a<T>>    { sletc v = true; };
template<class... T> struct o9_<o9v<T...>> { sletc v = true; };
template<class... T> struct o9_<o9m<T...>> { sletc v = true; };
template<class... T> struct o9_<o9t<T...>> { sletc v = true; };


}

#include "end.hh"


#endif
