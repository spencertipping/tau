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


// Monadic function adapter: converts a polymorphic unary fn
// to a visitor against πv
template<class T>
auto πmf(T &&f)
{
  return [f=fo<T>(f)]<class U>(πi &i, U &&x)
    -> If<Eq<De<U>, πv>,
          decltype(vi(fn {f, bi(f, i, _1)}, fo<U>(x)))>
    { return vi(fn {f, bi(f, i, _1)}, fo<U>(x)); };
}

template<class T>
auto πdf(T &&f)  // dyadic
{
  return [f=fo<T>(f)]<class U, class V>(πi &i, U &&x, V &&y)
    -> If<Eq<De<U>, πv>,
          decltype(vi(fn {f, bi(f, i, _1, _2)}, fo<U>(x), fo<V>(y)))>
    { return vi(fn {f, bi(f, i, _1, _2)}, fo<U>(x), fo<V>(y)); };
}

template<class T>
auto πtf(T &&f)  // triadic
{
  return [f=fo<T>(f)]<class U, class V, class W>
    (πi &i, U &&x, V &&y, W &&z)
    -> If<Eq<De<U>, πv>,
          decltype(vi(fn {f, bi(f, i, _1, _2, _3)}, fo<U>(x), fo<V>(y), fo<W>(z)))>
    { return vi(fn {f, bi(f, i, _1, _2, _3)}, fo<U>(x), fo<V>(y), fo<W>(z)); };
}


// Close over a functional expression, evaluate it eagerly, and
// pass the result into the monadic op function when evaluated.
template<class T>
πf πme(T &&f, πf &&x)
{
  return [f=πmf(fo<T>(f)), x=mo(x)](πi &i) -> πv
    { return f(i, x(i)); };
}

template<class T>
πf πde(T &&f, πf &&x, πf &&y)
{
  return [f=πdf(fo<T>(f)), x=mo(x), y=mo(y)](πi &i) -> πv
    { let a = x(i);
      let b = y(i);
      return f(i, mo(a), mo(b)); };
}

template<class T>
πf πte(T &&f, πf &&x, πf &&y, πf &&z)
{
  return [f=πtf(fo<T>(f)), x=mo(x), y=mo(y), z=mo(z)](πi &i) -> πv
    { let a = x(i);
      let b = y(i);
      let c = z(i);
      return f(i, mo(a), mo(b), mo(c)); };
}


// Dyadic and triadic lazy functions: eagerly examine the first operand
// and receive the second and third as lazy πfs.
template<class T>
πf πdl(T &&f, πf &&x, πf &&y)
{
  return [f=fo<T>(f), x=mo(x), y=mo(y)](πi &i) -> πv
    { return vi(bi(f, i, _1, y), x(i)); };
}

template<class T>
πf πtl(T &&f, πf &&x, πf &&y, πf &&z)
{
  return [f=fo<T>(f), x=mo(x), y=mo(y), z=mo(z)](πi &i) -> πv
    { return vi(bi(f, i, _1, y, z), x(i)); };
}


}

#include "end.hh"


#endif
