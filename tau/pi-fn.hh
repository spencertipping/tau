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
typedef F<πv(πi&)> πf;
typedef πf const   πfc;


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
      return vi([&](let &a, let &b) { return f(i, a, b); },
                a, b); };
}

template<class T>
πf πte(T &&f, πf &&x, πf &&y, πf &&z)
{
  return [f=fo<T>(f), x=mo(x), y=mo(y), z=mo(z)](πi &i) -> πv
    { let a = x(i);
      let b = y(i);
      let c = z(i);
      return vi([&](let &a, let &b, let &c) { return f(i, a, b, c); },
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
    { return vi([&](let &a) { return f(i, a, y); }, x(i)); };
}

template<class T>
πf πtl(T &&f, πf &&x, πf &&y, πf &&z)
{
  return [f=fo<T>(f), x=mo(x), y=mo(y), z=mo(z)](πi &i) -> πv
    { return vi([&](let &a) { return f(i, a, y, z); }, x(i)); };
}


}

#include "end.hh"


#endif
