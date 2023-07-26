#ifndef σpre_multiplex_h
#define σpre_multiplex_h

#include "../tau.hh"
#include "../tau/begin.hh"

namespace σ::pre
{

using namespace τ;


template<class A> struct mux_parsers
{
  Tx auto p(M<St, X>*) const
    { return φm(φn(φs("mux_parsers::p", φword_(), Rc<A const*>(this)->p(null<X>()))),
                [](Vc<T<φword, X>> &xs)
                  { M<St, X> r;
                    for (auto &[k, v] : xs) r[k.x] = v;
                    return r; }); }
};


}

#include "../tau/end.hh"

#endif
