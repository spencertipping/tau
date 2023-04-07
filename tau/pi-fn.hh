#ifndef τπfn_h
#define τπfn_h


#include "types.hh"
#include "pi-val.hh"
#include "pi-int.hh"

#include "begin.hh"

namespace τ
{


// π expression used to compute a value (this is where C++ type
// and dependency erasure happens)
//
// TODO: convert these things to proper C++ virtual structs
// so we have less std::function weirdness on wasm, and so we
// have more precise memory management

typedef F<πv(πi&)> πf;
typedef πf const   πfc;

typedef F<πv(πi&, πv&&)>             πmf;
typedef F<πv(πi&, πv&&, πv&&)>       πdf;
typedef F<πv(πi&, πv&&, πv&&, πv&&)> πtf;


// Compiler helpers: we want to compile around the erasure boundary.
// πmv(f) means "convert f to a function that evaluates + visits its argument".
// nmc(f, x) means "convert f() to close over x".
//
// Same for dyadic and triadic functions. Note that closures copy their operands.


template<class T>
πmf πmv(T &&f)
{ return [f=fo<T>(f)](πi &i, πv &&x) -> πv
  { return std::visit([&](auto &&x) { return f(i, x); }, x); }; }

template<class T>
πdf πdv(T &&f)
{ return [f=fo<T>(f)](πi &i, πv &&x, πv &&y) -> πv
  { return std::visit([&](auto &&x, auto &&y) { return f(i, x, y); }, x, y); }; }

template<class T>
πtf πtv(T &&f)
{ return [f=fo<T>(f)](πi &i, πv &&x, πv &&y, πv &&z) -> πv
  { return std::visit([&](auto &&x, auto &&y, auto &&z) { return f(i, x, y, z); }, x, y, z); }; }


inline πf πmc(πmf &&f, πfc &x)
{ return [f=mo(f), x](πi &i) -> πv { return f(i, x(i)); }; }

inline πf πdc(πdf &&f, πfc &x, πfc &y)
{ return [f=mo(f), x, y](πi &i) -> πv { return f(i, x(i), y(i)); }; }

inline πf πtc(πtf &&f, πfc &x, πfc &y, πfc &z)
{ return [f=mo(f), x, y, z](πi &i) -> πv
    { return f(i, x(i), y(i), z(i)); }; }


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
                        a, b); };
}

template<class T>
πf πte(T &&f, πf &&x, πf &&y, πf &&z)
{
  return [f=fo<T>(f), x=mo(x), y=mo(y), z=mo(z)](πi &i) -> πv
    { let a = x(i);
      let b = y(i);
      let c = z(i);
      return std::visit([&](let &a, let &b, let &c) { return f(i, a, b, c); },
                        a, b, c); };
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
    { return std::visit([&](let &a) { return f(i, a, y); }, x(i)); };
}

template<class T>
πf πtl(T &&f, πf &&x, πf &&y, πf &&z)
{
  return [f=fo<T>(f), x=mo(x), y=mo(y), z=mo(z)](πi &i) -> πv
    { return std::visit([&](let &a) { return f(i, a, y, z); }, x(i)); };
}


}

#include "end.hh"


#endif
