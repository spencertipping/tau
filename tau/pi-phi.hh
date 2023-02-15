#ifndef τπφ_h
#define τπφ_h

#include "phi.hh"
#include "phi-fn.hh"
#include "phi-str.hh"
#include "phi-ctor.hh"

#include "pi-int.hh"
#include "pi-fn.hh"

#include "begin.hh"

namespace τ
{


template<class T>
φ<πfn> φconst(φ<T> p)
{
  return φm<T, πfn>(p, [](T r)
    { return πinsn{
        (Ss{} << "const " << r).str(),
        [r](πi &i) { i.dpush(r); return πinsn_ok; }}; });
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
