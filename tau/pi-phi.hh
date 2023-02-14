#ifndef τπφ_h
#define τπφ_h

#include "phi.hh"
#include "phi-fn.hh"
#include "phi-str.hh"
#include "phi-ctor.hh"

#include "pi-int.hh"

#include "begin.hh"

namespace τ
{


// FIXME: do we want this value model, or do we want void(πi&) and mutable
// manipulation?

template<class T>
φ<πf<T>> φlift(φ<T> p)
{
  return φm<T, πf<T>>(p, [](T r) { return [r](πi) { return r; }; });
}


φ<i64>   φint_literal();
φ<f64>   φfloat_literal();
φ<St>    φstr_literal();
φ<η1sig> φsig_literal();

φ<uN>    φηtuple_key();
φ<St>    φηmap_key();


}

#include "end.hh"


#endif
