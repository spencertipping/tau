#ifndef τη1o_h
#define τη1o_h


#include <cstring>

#include "eta-types.hh"
#include "eta0o.hh"

#include "begin.hh"

namespace τ
{


template<class T>
η0o η1o(T *p, void(*xf)(T*) = nullptr)
{
  η0o r(η0t::η0);
  W(r.iptr(sizeof(void*)), 0, p);
  W(r.iptr(sizeof(void*)), 0, xf);
  return r;
}


inline η0o η1o(u64 x)
{ η0o r(η0t::uint_be);
  if      (ou<u32>(x)) W(r.iptr(8), 0, x);
  else if (ou<u16>(x)) W(r.iptr(4), 0, Sc<u32>(x));
  else if (ou<u8>(x))  W(r.iptr(2), 0, Sc<u16>(x));
  else                 W(r.iptr(1), 0, Sc<u8>(x));
  return r; }
inline η0o η1o(u32 x) { return η1o(Sc<u64>(x)); }
inline η0o η1o(u16 x) { return η1o(Sc<u64>(x)); }
inline η0o η1o(u8 x)  { return η1o(Sc<u64>(x)); }

inline η0o η1o(i64 x)
{ η0o r(η0t::int_be);
  if      (oi<i32>(x)) W(r.iptr(8), 0, x);
  else if (oi<i16>(x)) W(r.iptr(4), 0, Sc<i32>(x));
  else if (oi<i8>(x))  W(r.iptr(2), 0, Sc<i16>(x));
  else                 W(r.iptr(1), 0, Sc<i8>(x));
  return r; }
inline η0o η1o(i32 x) { return η1o(Sc<i64>(x)); }
inline η0o η1o(i16 x) { return η1o(Sc<i64>(x)); }
inline η0o η1o(i8 x)  { return η1o(Sc<i64>(x)); }

inline η0o η1o(f64 x) { η0o r(η0t::float_be); W(r.iptr(8), 0, x); return r; }
inline η0o η1o(f32 x) { return η1o(Sc<f64>(x)); }

inline η0o η1o(bool b) { η0o r(η0t::boolean); W(r.iptr(1), 0, Sc<u8>(b ? 1 : 0)); return r; }

inline η0o η1o(η1sig s) { η0o r(η0t::signal); W(r.iptr(1), 0, Sc<u8>(s)); return r; }

inline η0o η1o(Stc  &s) { η0o r(η0t::utf8);  memcpy(r.iptr(s.size()), s.data(), s.size()); return r; }
inline η0o η1o(Stvc &s) { η0o r(η0t::utf8);  memcpy(r.iptr(s.size()), s.data(), s.size()); return r; }
inline η0o η1o(Bc   &b) { η0o r(η0t::bytes); memcpy(r.iptr(b.size()), b.data(), b.size()); return r; }
inline η0o η1o(Bvc  &b) { η0o r(η0t::bytes); memcpy(r.iptr(b.size()), b.data(), b.size()); return r; }

inline η0o η1o(Ss const &s) { return η1o(s.str()); }

inline η0o η1o(chc *s) { return η1o(Stv{s}); }


inline η0o η1o(η0o const &x) { return x; }
inline η0o η1o(η0i const &x)
{
  auto r = η0o(x.type());
  r.c(x.c()).f(x.f()).h(x.h());
  x.into(r.iptr(x.osize()));
  return r;
}


inline void η1append(η0o &) {}

template<class X, class... Xs>
void η1append(η0o &o, X const &x, Xs const&... xs)
{
  o << η1o(x);
  η1append(o, xs...);
}


template<class... Xs> η0o η1t(Xs const&... xs) { η0o r(η0t::tuple); η1append(r, xs...); return r; }
template<class... Xs> η0o η1s(Xs const&... xs) { η0o r(η0t::set);   η1append(r, xs...); return r; }
template<class... Xs> η0o η1m(Xs const&... xs) { η0o r(η0t::map);   η1append(r, xs...); return r; }


template<class T> η0o η1o(Vc<T> &xs)
{
  η0o o(η0t::tuple);
  for (let &x : xs) o << η1o(x);
  return o;
}

template<class T> η0o η1o(S<T> const &xs)
{
  η0o o(η0t::set);
  for (let &x : xs) o << η1o(x);
  return o;
}

template<class K, class V> η0o η1o(M<K, V> const &xs)
{
  η0o o(η0t::map);
  for (let &[k, v] : xs) o << η1o(k) << η1o(v);
  return o;
}


inline η0o &operator<<(η0o &c, η0o const &i)
{
  A(η1tc[c.t()], "η₀o<<η with " << c.t());
  i.into(c.iptr(i.osize()));
  return i.f() ? c.f(true) : c;
}


template<class T> struct η0ob_ { sletc v = false; };

template<> struct η0ob_<St>   { sletc v = true; };
template<> struct η0ob_<Stc>  { sletc v = true; };
template<> struct η0ob_<Stv>  { sletc v = true; };
template<> struct η0ob_<Stvc> { sletc v = true; };
template<> struct η0ob_<B>    { sletc v = true; };
template<> struct η0ob_<Bc>   { sletc v = true; };
template<> struct η0ob_<Bv>   { sletc v = true; };
template<> struct η0ob_<Bvc>  { sletc v = true; };

template<class T> concept η0ob = η0ob_<T>::v;


template<η0ob T>
η0o &operator<<(η0o &s, T const &x)
{
  A(η1tb[s.t()], "η₀o<<B with " << s.t());
  memcpy(s.iptr(x.size()), x.data(), x.size());
  return s;
}

inline η0o &operator<<(η0o &s, chc *x)
{
  return s << Stvc{x, strlen(x)};
}


}

#include "end.hh"

#endif
