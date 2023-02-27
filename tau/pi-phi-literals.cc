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

static φ<u64> int_suffix()
{ return φa<u64>(φl("K", Sc<u64>(1) << 10),
                 φl("M", Sc<u64>(1) << 20),
                 φl("G", Sc<u64>(1) << 30),
                 φl("T", Sc<u64>(1) << 40),
                 φl("P", Sc<u64>(1) << 50)); }


static φ<i64> e10_literal()
{ return φm<V<St>, i64>(φre("E(\\d+)"), [](V<St> x)
  { i64 n; Ss{x[1]} >> n;
    i64 r = 1;
    for (iN i = 0; i < n; ++i) r *= 10;
    return r; }); }

static φ<i64> e2_literal()
{ return φm<V<St>, i64>(φre("B(\\d+)"), [](V<St> x)
  { i64 n; Ss{x[1]} >> n; return Sc<i64>(1) << n; }); }


φ<i64> φint_literal()
{ return φa<i64>(e10_literal(),
                 e2_literal(),
                 φm<P<V<St>, Op<u64>>, i64>(
                   φs(φre("-?[0-9]+"), φo(int_suffix())),
                   [](P<V<St>, Op<u64>> v)
                     { i64 x = 0; Ss{std::get<0>(v)[0]} >> x;
                       return x * std::get<1>(v).value_or(1); })); }

φ<f64> φfloat_literal()
{ return φm<V<St>, f64>(
    φre(R"(-?(?:[0-9]+\.[0-9]*(?:[eE]?[-+]?[0-9]+)?|\.[0-9]+(?:[eE]?[-+]?[0-9]+)?))"),
    [](V<St> v) { f64 x = 0; Ss{v[0]} >> x; return x; }); }

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
    r = φa<πfn>(φk(φfloat_literal()),
                φk(φint_literal()),
                φk(φstr_literal()),
                φk(φsig_literal()));
  return r;
}


}

#include "end.hh"
