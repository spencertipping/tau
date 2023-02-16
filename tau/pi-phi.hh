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
φ<πfn>    φγ1();
φd_<πfn> &φγd();


// Parser for π η-transformers (expressions/programs)
φ<πfn> φπ();
φ<πfn> φπfn();


φ<i64>   φint_literal();
φ<f64>   φfloat_literal();
φ<St>    φstr_literal();
φ<η1sig> φsig_literal();

φ<πfn>   φπ_literal();


φ<πfn>   φtuple();
φ<πfn>   φmap();
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


// Append an instruction
φ<πfn> φinsn(φ<πfn>, πinsn);
φ<πfn> φnull(πinsn);


template<class... Xs>
φ<πfn> φπs(Xs const&... ps)
{
  return φm<V<πfn>, πfn>(φS<πfn>(ps...), [](V<πfn> fs)
    { πfn r;
      for (let &f : fs) r += f;
      return r; });
}


// Convert a constant into a πfn that pushes that constant
template<class T> φ<πfn> φk(φ<T> p)
{ return φm<T, πfn>(p, [](T r)
  { return πinsn{
      (Ss{} << "const " << r).str(),
      [r](πi &i) { i.dpush(η1o(r)); }}; }); }


φ<πfn> φword();


}

#include "end.hh"


#endif
