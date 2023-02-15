#ifndef τπφ_h
#define τπφ_h

#include "eta.hh"

#include "phi.hh"
#include "phi-fn.hh"
#include "phi-str.hh"
#include "phi-ctor.hh"

#include "pi-int.hh"
#include "pi-fn.hh"

#include "begin.hh"

namespace τ
{


// Parser for all τ-defined γs
φ<πfn> φγ();

// Parser for π η-transformers (expressions/programs)
φ<πfn> φπ();


// Primary dispatch for γ parser
φd_<πfn> &φγd();


// Convert a constant into a πfn that pushes that constant
template<class T> φ<πfn> φk(φ<T> p)
{ return φm<T, πfn>(p, [](T r)
    { return πinsn{
        (Ss{} << "const " << r).str(),
        [r](πi &i) { i.dpush(η1o(r)); return πinsn_ok; }}; }); }


φ<i64>   φint_literal();
φ<f64>   φfloat_literal();
φ<St>    φstr_literal();
φ<η1sig> φsig_literal();

φ<πfn>   φπ_literal();

φ<uN>    φηtuple_key();
φ<St>    φηmap_key();


φ<πfn>   φtuple();
φ<πfn>   φηtuple_element();
φ<πfn>   φηmap_element();


// Whitespace + comments
φ<πfn>   φws();
φ<πfn>   φws(φ<πfn>);  // whitespace wrapped around something


// A value that is parsed independently of the current input type
φ<πfn>    φatom();
φa_<πfn> &φatoma();
φd_<πfn> &φatomd();


// Postfix operator clause, applied to a value
φ<πfn>    φop();
φd_<πfn> &φopd();


}

#include "end.hh"


#endif
