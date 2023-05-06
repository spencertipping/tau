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

Tt πmf πmv(T &&f)
{
  return [f=fo<T>(f)](πi &i, πv &&x) -> πv
    { return std::visit([&](auto &&x) { return f(i, mo(x)); }, x.v_); };
}

Tt πdf πdv(T &&f)
{
  return [f=fo<T>(f)](πi &i, πv &&x, πv &&y) -> πv
    { return std::visit([&](auto &&x, auto &&y) { return f(i, mo(x), mo(y)); },
                        x.v_, y.v_); };
}


inline πf πvq(πvc &x)
{
  return [=](πi &i) -> πv { return x; };
}

inline πf πmc(πmf const &f, πf const &x)
{
  return [=](πi &i) -> πv { return f(i, x(i)); };
}

inline πfs πmc(πmsf const &f, πf const &x)
{
  return [=](πi &i) -> πvs { return f(i, x(i)); };
}

inline πf πdc(πdf const &f, πf const &x, πf const &y)
{
  return [=](πi &i) -> πv { return f(i, x(i), y(i)); };
}

inline πfs πdc(πdsf const &f, πf const &x, πf const &y)
{
  return [=](πi &i) -> πvs { return f(i, x(i), y(i)); };
}

// NOTE: triadic operators are by default lazy when closed
inline πf πtc(πtf const &f, πf const &x, πf const &y, πf const &z)
{
  return [=](πi &i) -> πv { return f(i, x, y, z); };
}

inline πfs πtc(πtsf const &f, πf const &x, πf const &y, πf const &z)
{
  return [=](πi &i) -> πvs { return f(i, x, y, z); };
}


// Eager evaluator+visitors for arguments. We define these only for
// monadic/dyadic, not triadic, to avoid the humongous dispatch tables
// that triadic operators would produce.

Tt πf πme(T &&f, πf &&x)
{
  return [f=fo<T>(f), x=mo(x)](πi &i) -> πv
    { return f(i, x(i)); };
}

Tt πf πde(T &&f, πf &&x, πf &&y)
{
  return [f=fo<T>(f), x=mo(x), y=mo(y)](πi &i) -> πv
    { let a = x(i);
      let b = y(i);
      return std::visit([&](let &a, let &b) { return f(i, a, b); },
                        a.v_, b.v_); };
}


// Lazy dispatching variants
Tt πf πml(T &&f, πf &&x)
{
  return [f=fo<T>(f), x=mo(x)](πi &i) -> πv
    { return f(i, x); };
}

Tt πf πdl(T &&f, πf &&x, πf &&y)
{
  return [f=fo<T>(f), x=mo(x), y=mo(y)](πi &i) -> πv
    { return std::visit([&](auto &&a) { return f(i, mo(a), y); }, x(i).v_); };
}


}

#include "end.hh"


#endif
