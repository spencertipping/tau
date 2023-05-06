#include <numeric>

#include "types.hh"
#include "pi-phi.hh"
#include "pi-fn.hh"
#include "phi-ctor.hh"
#include "phi-fn.hh"
#include "phi-str.hh"
#include "pi-fn-eta.hh"

#include "begin.hh"

namespace τ
{


φ<πf> πφcore_a()
{
  slet r = φd<πf>("πcore_a",
                  "A", φR<πf>([](πi &i) { return i.y(); }),
                  "B", φR<πf>([](πi &i) { return i.y()[1]; }),
                  "C", φR<πf>([](πi &i) { return i.y()[2]; }),
                  "D", φR<πf>([](πi &i) { return i.y()[3]; }),
                  "E", φR<πf>([](πi &i) { return i.y()[4]; }),
                  "F", φR<πf>([](πi &i) { return i.y()[5]; }),
                  "G", φR<πf>([](πi &i) { return i.y()[6]; }),
                  "H", φR<πf>([](πi &i) { return i.y()[7]; }),
                  "",  φm(πφlit(), πvq));
  return r;
}


φ<πmf> πφcore_m()
{
  slet r = φd<πmf>("πcore_m",
                   "sin",   πηsin(),
                   "cos",   πηcos(),
                   "tan",   πηtan(),
                   "asin",  πηasin(),
                   "acos",  πηacos(),
                   "atan",  πηatan(),
                   "sinh",  πηsinh(),
                   "cosh",  πηcosh(),
                   "tanh",  πηtanh(),
                   "asinh", πηasinh(),
                   "acosh", πηacosh(),
                   "atanh", πηatanh(),

                   "erf",    πηerf(),
                   "erfc",   πηerfc(),
                   "tgamma", πηtgamma(),
                   "lgamma", πηlgamma(),

                   "sqrt",  πηsqrt(),
                   "cbrt",  πηcbrt(),
                   "exp",   πηexp(),
                   "exp2",  πηexp2(),
                   "expm1", πηexpm1(),
                   "log",   πηlog(),
                   "log2",  πηlog2(),
                   "log1p", πηlog1p(),

                   "ceil",  πηceil(),
                   "floor", πηfloor(),
                   "trunc", πηtrunc(),
                   "round", πηround(),

                   "~",  πηinv(),
                   "-",  πηneg(),
                   "!",  πηnot(),
                   "!!", πηnotnot());
  return r;
}


φ<πdf> πφcore_d()
{
  slet r = φd<πdf>("πcore_d",
                   "+",  πηadd(),
                   "-",  πηsub(),
                   "**", πηpow(),
                   "*",  πηmul(),
                   "//", πηdiv(),

                   "%",  πηmod(),
                   "<<", πηlsh(),
                   ">>", πηrsh(),
                   "&",  πηand(),
                   "|",  πηor(),
                   "^",  πηxor(),

                   "<=", πηle(),
                   ">=", πηge(),
                   "<>", πηcmp(),
                   "<",  πηlt(),
                   ">",  πηgt(),
                   "=",  πηeq(),
                   "!=", πηne(),

                   "atan2",    πηatan2(),
                   "hypot",    πηhypot(),
                   "logn",     πηlogn(),
                   "copysign", πηcopysign());
  return r;
}


φ<πtf> πφcore_t()
{
  slet r = φd<πtf>("πcore_t", "?", πηcond());
  return r;
}


φ<πmsf> πφcore_ms() { slet r = φF<πmsf>(); return r; }
φ<πdsf> πφcore_ds() { slet r = φF<πdsf>(); return r; }
φ<πtsf> πφcore_ts() { slet r = φF<πtsf>(); return r; }

φ<πsmf>  πφcore_sm() { slet r = φF<πsmf>(); return r; }
φ<πsmsf> πφcore_sms() { slet r = φF<πsmsf>(); return r; }


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


φ<πname> πφname()
{
  slet r = φ2("name", φl("'"),
              φa("name",
                 φm(πφstr(), [](St x) { return πname{x}; }),
                 φm(φcs("abcdefghjiklmnopqrstuvxyz", false, 1), [](St x) { return πname{x}; })));
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

φ<πv> πφlit()
{
  slet r = φa<πv>("lit",
                  φm(πφfloat(), [](f64 x)   { return πv{x}; }),
                  φm(πφint(),   [](i64 x)   { return πv{x}; }),
                  φm(πφname(),  [](πname x) { return πv{x}; }),
                  φm(πφstr(),   [](St x)    { return πv{x}; }));
  return r;
}


φ<πfs> πφfs(φ<πfs> f)
{
  return φm(φn(f), [](V<πfs> &&x) -> πfs
    { return [x = mo(x)](πi &i)
      { πvs r;
        for (let &p : x) for (let &y : p(i)) r.push_back(y);
        return r; }; });
}


T<φ<πf>, φ<πf>, φ<πfs>>
πφ(φ<πf> a, φ<πmf> m, φ<πdf> d, φ<πtf> t,
   φ<πmsf> ms, φ<πdsf> ds, φ<πtsf> ts,
   φ<πsmf> sm, φ<πsmsf> sms)
{
  let pa  = φa0<πf> ("πa");  auto &a_ = pa .as<φa_<πf>>();
  let ps  = φa0<πf> ("πs");  auto &s_ = ps .as<φa_<πf>>();
  let pp_ = φa0<πfs>("πp");  auto &p_ = pp_.as<φa_<πfs>>();
  let pp  = πφfs(pp_);

  let aw = φW(pa);
  let sw = φW(ps);
  let pw = φW(pp);

  let te = φ2("()", πφlp(), φm(pw, [](πfs &&f) -> πf
    { return [f=mo(f)](πi &i) -> πv
      { return {Sp<V<πv>>{new V<πv>{f(i)}}}; }; }), φo(πφrp()));

  a_ << πφgroup(sw) << te << πφwrap(a);

  s_ << φm(φs("mf",     πφwrap(m), sw),     [](let &r) -> πf { let &[m, x]       = r; return πmc(m, x); })
     << φm(φs("df", aw, πφwrap(d), sw),     [](let &r) -> πf { let &[x, d, y]    = r; return πdc(d, x, y); })
     << φm(φs("tf", aw, πφwrap(t), sw, sw), [](let &r) -> πf { let &[x, t, y, z] = r; return πtc(t, x, y, z); })
     << aw;

  p_ << φm(φs("ms",     πφwrap(ms), sw),     [](let &r) -> πfs { let &[m, x]       = r; return πmc(m, x); })
     << φm(φs("ds", aw, πφwrap(ds), sw),     [](let &r) -> πfs { let &[x, d, y]    = r; return πdc(d, x, y); })
     << φm(φs("ts", aw, πφwrap(ts), sw, sw), [](let &r) -> πfs { let &[x, t, y, z] = r; return πtc(t, x, y, z); })
     << φm(sw,                               [](let &f) -> πfs
       { return [f](πi &i) { πvs r; r.push_back(f(i)); return r; }; });

  return {pa, ps, pp};
}


}

#include "end.hh"
