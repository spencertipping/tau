#include "tau-phi.hh"
#include "begin.hh"

namespace τ
{


O &operator<<(O &s, φig)             { return s << "φig"; }
O &operator<<(O &s, φident const &x) { return s << x.x; }
O &operator<<(O &s, φword  const &x) { return s << x.x; }


φ<St> φws_() { slet r = φcs(" \t\n\r", false, 1);                      return r; }
φ<St> φlc_() { slet r = φq("lc", φl("# ", ""), φcs("\n", true));       return r; }
φ<St> φig_() { slet r = φq("ign", φn(φa<St>("ign0", φws_(), φlc_()))); return r; }

φ<St> φlb_() { slet r = φwrap(φl("[")); return r; }
φ<St> φrb_() { slet r = φwrap(φl("]")); return r; }
φ<St> φlp_() { slet r = φwrap(φl("(")); return r; }
φ<St> φrp_() { slet r = φwrap(φl(")")); return r; }
φ<St> φlB_() { slet r = φwrap(φl("{")); return r; }
φ<St> φrB_() { slet r = φwrap(φl("}")); return r; }

φ<St> φco_() { slet r = φwrap(φl(",")); return r; }


φ<i64> φint_()     { slet r = φa("int", φint_hex_(), φint_bin_(), φint_oct_(), φint_dec_()); return r; }
φ<i64> φint_dec_() { slet r = φm(φq("idec", φo(φl("-")),           φcs("0123456789",             false, 1)), [](St x) -> i64 { return std::stoll(x, nullptr, 10); }); return r; }
φ<i64> φint_oct_() { slet r = φm(φq("ioct", φo(φl("-")), φl("0"),  φcs("01234567",               false, 1)), [](St x) -> i64 { return std::stoll(x, nullptr, 8);  }); return r; }
φ<i64> φint_bin_() { slet r = φm(φ2("ibin",              φl("0b"), φcs("01",                     false, 1)), [](St x) -> i64 { return std::stoll(x, nullptr, 2);  }); return r; }
φ<i64> φint_hex_() { slet r = φm(φq("ihex", φo(φl("-")), φl("0x"), φcs("0123456789abcdefABCDEF", false, 1)), [](St x) -> i64 { return std::stoll(x, nullptr, 16); }); return r; }

φ<f64> φfloat_()
{
  slet ds  = φcs("0123456789", false, 1, 100);
  slet exp = φq("fexp", φcs("eE", false, 1, 1), φcs("-+", false, 0, 1), ds);
  slet r   = φm(φq("float",
                   φo(φl("-")),
                   φa("float0",
                      φq("f1", ds, φl("."), φo(φs("f1a", ds, φo(exp)))),
                      φq("f2",     φl("."),    φs("f2a", ds, φo(exp))),
                      φq("f3", ds,                              exp))),
                [](St x) { return std::stod(x); });
  return r;
}


φ<φword> φword_()
{
  slet r = φm(φre("[a-z][a-z_0-9]*"),
              [](Vc<St> &x) { return φword{x.front()}; });
  return r;
}


φ<φident> φident_()
{
  slet r = φm(φre("[a-z][a-z_0-9']*"),
              [](Vc<St> &x) { return φident{x.front()}; });
  return r;
}


φ<ηname> φname_()
{
  slet r = φ2("name", φl("'"),
              φa("name",
                 φm(φstr_(),  [](St    &&x) { return ηname{mo(x)}; }),
                 φm(φword_(), [](φword &&x) { return ηname{mo(x).x}; })));
  return r;
}

φ<St> φstr_escape_()
{
  slet r = φa("sesc",
              φl("\\"),
              φl("\""),
              φl<St>("n", "\n"),
              φl<St>("r", "\r"),
              φl<St>("t", "\t"));
  return r;
}

φ<St> φstr_()
{
  slet r = φm(φ2("str",
                 φl("\""),
                 φn(φa("schar", φcs("\\\"", true, 1), φ2("sesc", φl("\\"), φstr_escape_()))),
                 φl("\"")),
              [](V<St> xs)
                { let n = std::accumulate(xs.begin(), xs.end(), 0, [](i64 a, Stc &b) { return a + b.size(); });
                  St  r; r.reserve(n);
                  for (let &x : xs) r.append(x);
                  return r; });
  return r;
}


}

#include "end.hh"
