#include <numeric>

#include "types.hh"
#include "pi-phi.hh"
#include "phi-ctor.hh"
#include "phi-fn.hh"
#include "phi-str.hh"

#include "begin.hh"

namespace τ
{


πφ_::πφ_()
  : se_ (φa0<π1>("πse")),
    pe_ (φa0<π1>("πpe")),
    s_  (φa0<π1>("πs")),
    p_  (φa0<π1>("πp")),
    ssp_(φd <π0>("πssp")),
    psp_(φd <π0>("πpsp")),
    spp_(φd <π0>("πspp")),
    ppp_(φd <π0>("πppp")),
    sp_ (φd <π0>("πsp")),
    pp_ (φd <π0>("πpp"))
{
  let s1 = s_ | (ssp_ & φW(se_)) % pre;  // s_atom | ss_pre s
  let s2 =      (psp_ & φW(pe_)) % pre;  // ps_pre p
  let s3 = (s1 & φn(sp_)) % post;        // (s_atom | ss_pre s) s_post*
  se_.as<φa_<π1>>() << s3 << s2;

  let p1 = p_
    | (spp_ & φW(se_)) % pre
    | (ppp_ & φW(pe_)) % pre;
  let p2 = (p1 & φn(pp_)) % post;
  pe_.as<φa_<π1>>() << p1 << p2;

  def_sa(πφgroup(se_));
  def_pa(πφgroup(pe_));

  // TODO: (s ','?)* case
  // TODO: '(' p ')' case
}


φ<St> πφws() { slet r = φcs(" \t\n\r", false, 1);                      return r; }
φ<St> πφlc() { slet r = φq("lc", φl("# ", ""), φcs("\n", true));       return r; }
φ<St> πφig() { slet r = φq("ign", φn(φa<St>("ign0", πφws(), πφlc()))); return r; }

φ<St> πφlb() { slet r = πφwrap(φl("[")); return r; }
φ<St> πφrb() { slet r = πφwrap(φl("]")); return r; }

φ<St> πφlp() { slet r = πφwrap(φl("(")); return r; }
φ<St> πφrp() { slet r = πφwrap(φl(")")); return r; }


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


/*
φ<πfs> πφfs(φ<πfs> f)
{
  return φm(φn(f), [](V<πfs> &&x) -> πfs
    { return [x = mo(x)](πi &i)
      { πvs r;
        for (let &p : x) for (let &y : p(i)) r.push_back(y);
        return r; }; });
}
*/


}

#include "end.hh"
