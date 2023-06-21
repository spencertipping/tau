#include <numeric>

#include "types.hh"
#include "pi-phi.hh"
#include "phi-ctor.hh"
#include "phi-fn.hh"
#include "phi-str.hh"

#include "begin.hh"

namespace τ
{


static π1 np(V<π1> &&ps)
{
  Ss ns;
  bool first = true;
  for (let &p : ps)
  {
    if (first) first = false;
    else       ns << "|";
    ns << p;
  }

  return π1{ns.str(), [ps=mo(ps)](πi &i)
    {
      for (let &p : ps) p(i);  // push each onto stack in reverse order
      // TODO

    }};

}


φ<π1> πφnp(φ<π1> p)
{
  slet r = φm(φn(φ1("p','?", p, φo(πφco()))), np);
  return r;
}


φ<St> πφws() { slet r = φcs(" \t\n\r", false, 1);                      return r; }
φ<St> πφlc() { slet r = φq("lc", φl("# ", ""), φcs("\n", true));       return r; }
φ<St> πφig() { slet r = φq("ign", φn(φa<St>("ign0", πφws(), πφlc()))); return r; }

φ<St> πφlb() { slet r = πφwrap(φl("[")); return r; }
φ<St> πφrb() { slet r = πφwrap(φl("]")); return r; }
φ<St> πφlp() { slet r = πφwrap(φl("(")); return r; }
φ<St> πφrp() { slet r = πφwrap(φl(")")); return r; }
φ<St> πφlB() { slet r = πφwrap(φl("{")); return r; }
φ<St> πφrB() { slet r = πφwrap(φl("}")); return r; }

φ<St> πφco() { slet r = πφwrap(φl(",")); return r; }


φ<i64> πφint()     { slet r = φa("int", πφint_hex(), πφint_bin(), πφint_oct(), πφint_dec()); return r; }
φ<i64> πφint_dec() { slet r = φm(φq("idec", φo(φl("-")),           φcs("0123456789",             false, 1)), [](St x) -> i64 { return std::stoll(x, nullptr, 10); }); return r; }
φ<i64> πφint_oct() { slet r = φm(φq("ioct", φo(φl("-")), φl("0"),  φcs("01234567",               false, 1)), [](St x) -> i64 { return std::stoll(x, nullptr, 8);  }); return r; }
φ<i64> πφint_bin() { slet r = φm(φ2("ibin",              φl("0b"), φcs("01",                     false, 1)), [](St x) -> i64 { return std::stoll(x, nullptr, 2);  }); return r; }
φ<i64> πφint_hex() { slet r = φm(φq("ihex", φo(φl("-")), φl("0x"), φcs("0123456789abcdefABCDEF", false, 1)), [](St x) -> i64 { return std::stoll(x, nullptr, 16); }); return r; }

φ<f64> πφfloat()
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


φ<St> πφword()
{
  slet r = φcs("abcdefghjiklmnopqrstuvxyz", false, 1);
  return r;
}


φ<πident> πφident()
{
  slet r = φm(πφwrap(φre("[a-z][a-z_0-9']*")),
              [](Vc<St> &x) { return πident{x.front()}; });
  return r;
}


φ<ηname> πφname()
{
  slet r = φ2("name", φl("'"),
              φa("name",
                 φm(πφstr(),  [](St x) { return ηname{x}; }),
                 φm(πφword(), [](St x) { return ηname{x}; })));
  return r;
}

φ<St> πφstr_escape()
{
  slet r = φa("sesc",
              φl("\\"),
              φl("\""),
              φl<St>("n", "\n"),
              φl<St>("r", "\r"),
              φl<St>("t", "\t"));
  return r;
}

φ<St> πφstr()
{
  slet r = φm(φ2("str",
                 φl("\""),
                 φn(φa("schar", φcs("\\\"", true, 1), φ2("sesc", φl("\\"), πφstr_escape()))),
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
