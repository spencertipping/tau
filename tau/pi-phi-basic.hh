#ifndef τπφbasic_h
#define τπφbasic_h

#include "types.hh"
#include "phi.hh"
#include "phi-ctor.hh"
#include "phi-auto.hh"
#include "pi-phi-markers.hh"
#include "pi-fn.hh"
#include "tau-phi.hh"

#include "begin.hh"

namespace τ
{


// Splice one or more η-producing elements into a single η
π1 πφnp(V<π1>&&);


template<class A> struct πφP
{
  Tt auto p(πP<T>*) const
    { return φm(Sc<A const*>(this)->p(null<T>()), [](auto &&x) { return πP{x}; }); }
};


}

#include "end.hh"

#endif
