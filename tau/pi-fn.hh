#ifndef τπfn_h
#define τπfn_h


#include "types.hh"
#include "pi-val.hh"
#include "pi-int.hh"

#include "begin.hh"

namespace τ
{


// Compiler helpers: we want to compile around the erasure boundary.
// πmv(f) means "convert f to a function that evaluates + visits its argument".
// nmc(f, x) means "convert f() to close over x".
//
// Same for dyadic and triadic functions. Note that closures copy their operands.
//
// We don't define πtv because it would produce an enormous dispatch table
// from cubic std::visit complexity (against three variants). Realistically,
// two variants is the dispatch limit if we want any reasonable compile times
// and binary sizes.

template<class T>
πmf πmv(T &&f)
{
  return [f=fo<T>(f)](πi &i, πv &&x) -> πv
  { return std::visit([&](auto &&x) { return f(i, x); }, x.v_); };
}

template<class T>
πdf πdv(T &&f)
{
  return [f=fo<T>(f)](πi &i, πv &&x, πv &&y) -> πv
  { return std::visit([&](auto &&x, auto &&y) { return f(i, x, y); },
                      x.v_, y.v_); };
}


inline πf πmc(πmf &&f, πfc &x)
{
  return [f=mo(f), x](πi &i) -> πv { return f(i, x(i)); };
}

inline πf πdc(πdf &&f, πfc &x, πfc &y)
{
  return [f=mo(f), x, y](πi &i) -> πv { return f(i, x(i), y(i)); };
}

inline πf πtc(πtf &&f, πfc &x, πfc &y, πfc &z)
{
  return [f=mo(f), x, y, z](πi &i) -> πv { return f(i, x(i), y(i), z(i)); };
}


// There are two kinds of functions in π: lazy and eager. Lazy functions
// examine only the first argument and leave the other(s) unevaluated.
// Eager functions evaluate all arguments in order and visit them
// jointly.
//
// C++/π translation functions are called πXY, where X is "m" for
// monadic, "d" for dyadic, and "t" for triadic. Y is "l" for lazy and
// "e" for eager.
//
// Any eager arguments are visited, meaning we accept overloaded
// visitor functions for them.
//
// As above, no πte because the std::visit dispatch table size would be
// too large.

template<class T>
πf πme(T &&f, πf &&x)
{
  return [f=fo<T>(f), x=mo(x)](πi &i) -> πv
    { return f(i, x(i)); };
}

template<class T>
πf πde(T &&f, πf &&x, πf &&y)
{
  return [f=fo<T>(f), x=mo(x), y=mo(y)](πi &i) -> πv
    { let a = x(i);
      let b = y(i);
      return std::visit([&](let &a, let &b) { return f(i, a, b); },
                        a.v_, b.v_); };
}


template<class T>
πf πml(T &&f, πf &&x)
{
  return [f=fo<T>(f), x=mo(x)](πi &i) -> πv
    { return f(i, x); };
}

template<class T>
πf πdl(T &&f, πf &&x, πf &&y)
{
  return [f=fo<T>(f), x=mo(x), y=mo(y)](πi &i) -> πv
    { return std::visit([&](let &a) { return f(i, a, y); }, x(i).v_); };
}

template<class T>
πf πtl(T &&f, πf &&x, πf &&y, πf &&z)
{
  return [f=fo<T>(f), x=mo(x), y=mo(y), z=mo(z)](πi &i) -> πv
    { return std::visit([&](let &a) { return f(i, a, y, z); }, x(i).v_); };
}


}

#include "end.hh"


#endif
