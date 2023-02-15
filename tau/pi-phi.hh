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
φ<πfn>    φγ();

// Parser for π η-transformers
φ<πfn>    φπ();


// Primary dispatch for γ parser
φd_<πfn> &φγd();


// Primary alternative for π parser
φa_<πfn> &φπa();

// Secondary dispatch for π parser
φd_<πfn> &φπd();


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
φ<πfn>   φπatom();


// TODO: typed expression
// TODO: generic expression
// TODO: interpreter-type dispatch parser


}

#include "end.hh"


#endif
