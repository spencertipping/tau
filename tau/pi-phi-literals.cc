#include "pi-phi.hh"
#include "begin.hh"

namespace τ
{


static φ<St> sq_str()
{ return φm<P<int, St>, St>(
    φs(φl("'", 0), φcs("\t\n []\"'\\", true)),
    [](auto p) { return std::get<1>(p); }); }

static φ<St> dq_str()
{ return φm<P<P<int, St>, int>, St>(
    φs(φs(φl("\"", 0), φcs("\"\\", true)), φl("\"", 0)),
    [](auto t) { return std::get<1>(std::get<0>(t)); }); }


φ<i64> φint_literal()
{ return φm<St, i64>(φcs("0123456789"),
                     [](St v) { i64 x; Ss{v} >> x; return x; }); }

φ<f64> φfloat_literal()
{ return φm<St, f64>(φcs("0123456789.eE+-"),
                     [](St v) { f64 x; Ss{v} >> x; return x; }); }

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
  return φa<πfn>(φk(φint_literal()),
                 φk(φfloat_literal()),
                 φk(φstr_literal()),
                 φk(φsig_literal()));
}


φ<uN> φηtuple_key()
{ return φm<St, uN>(φcs("abcdefghij", false, 1),
                    [](St k) { return k[0] - 'a'; }); }

φ<St> φηmap_key()
{ return φcs("abcdefghijklmnopqrstuvwxyz"); }


}

#include "end.hh"
