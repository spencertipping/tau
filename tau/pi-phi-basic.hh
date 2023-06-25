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


// Take a single plural-element context and parse multiple of them,
// splicing the results into a single output η stream.
φ<π1> πφnp(φ<π1>);


struct πφP
{
  Tt auto p(πP<T>*) const
    { return φm(p(null<T>()), [](auto &&x) { return πP{x}; }); }
};


}

#include "end.hh"

#endif
