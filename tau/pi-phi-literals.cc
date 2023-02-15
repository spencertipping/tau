#include "pi-phi.hh"
#include "begin.hh"

namespace τ
{


static φ<St> sq_str()
{ return φm<P<int, St>, St>(
    φs(φl("'", 0), φcs("\t\n []\"'\\{}()", true)),
    [](auto p) { return std::get<1>(p); }); }

static φ<St> dq_str()
{ return φm<P<P<int, St>, int>, St>(
    φs(φs(φl("\"", 0), φcs("\"\\", true)), φl("\"", 0)),
    [](auto t) { return std::get<1>(std::get<0>(t)); }); }


φ<i64> φint_literal()
{ return φm<St, i64>(φcs("0123456789-", false, 1),
                     [](St v) { i64 x = 0; Ss{v} >> x; return x; }); }

// FIXME: floats are parsed preferentially as ints, followed by a float piece.
// We need to unify these parsers, most likely.
φ<f64> φfloat_literal()
{ return φm<St, f64>(φcs("0123456789.-", false, 1),
                     [](St v) { f64 x = 0; Ss{v} >> x; return x; }); }

φ<St> φstr_literal()
{ return φa<St>(sq_str(), dq_str()); }

φ<η1sig> φsig_literal()
{ return φa<η1sig>(φl("α", η1sig::α),
                   φl("κ", η1sig::κ),
                   φl("τ", η1sig::τ),
                   φl("ι", η1sig::ι),
                   φl("ω", η1sig::ω)); }


φ<πfn> φπ_literal()
{
  static φ<πfn> r;
  if (!r)
    r = φa<πfn>(φk(φint_literal()),
                φk(φfloat_literal()),
                φk(φstr_literal()),
                φk(φsig_literal()));
  return r;
}


}

#include "end.hh"
