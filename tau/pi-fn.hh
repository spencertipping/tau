#ifndef τπfn_h
#define τπfn_h


#include "pi-val.hh"
#include "pi-int.hh"

#include "begin.hh"

namespace τ
{


// π expression used to compute a value
typedef F<πv(πi&)> πf;


// Unary function adapter: converts a polymorphic unary fn
// to a visitor against πv
template<class T>
auto πun(T &&f)
{
  return [f=fo<T>(f)]<class U>(πi &i, U &&x)
    -> If<Eq<De<U>, πv>,
          decltype(vi(fn {f, bi(f, i)},
                      fo<decltype(Dv<U>().v)>(x.v)))>
    {
      return vi(fn {f, bi(f, i)},
                fo<decltype(Dv<U>().v)>(x.v));
    };
}


// Binary function adapter: converts a polymorphic binary fn
// to a visitor against πv pairs
template<class T>
auto πbin(T &&f)
{
  return [f=fo<T>(f)]<class U, class V>(πi &i, U &&x, V &&y)
    -> If<Eq<De<U>, πv>,
          decltype(vi(fn {f, bi(f, i)},
                      fo<decltype(Dv<U>().v)>(x.v),
                      fo<decltype(Dv<V>().v)>(y.v)))>
    {
      return vi(fn {f, bi(f, i)},
                fo<decltype(Dv<U>().v)>(x.v),
                fo<decltype(Dv<V>().v)>(y.v));
    };
}


}

#include "end.hh"


#endif
