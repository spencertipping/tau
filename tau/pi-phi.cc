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
                  "y", φR<πf>([](πi &i) { return i.y(); }),
                  "A", φR<πf>([](πi &i) { return i.y(); }),
                  "B", φR<πf>([](πi &i) { return i.y()[1]; }),
                  "C", φR<πf>([](πi &i) { return i.y()[2]; }),
                  "D", φR<πf>([](πi &i) { return i.y()[3]; }),
                  "E", φR<πf>([](πi &i) { return i.y()[4]; }),
                  "F", φR<πf>([](πi &i) { return i.y()[5]; }),
                  "G", φR<πf>([](πi &i) { return i.y()[6]; }),
                  "H", φR<πf>([](πi &i) { return i.y()[7]; }),
                  "$", φm(πφword(), [](St n) -> πf
                    { return [=](πi &i) { return i.vs[n]; }; }),
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


φ<πmsf> πφcore_ms()
{
  slet r = φd<πmsf>("πcore_ms", "@", πηflatten());
  return r;
}


φ<πdsf> πφcore_ds() { slet r = φF<πdsf>(); return r; }
φ<πtsf> πφcore_ts() { slet r = φF<πtsf>(); return r; }

φ<πsmf>  πφcore_sm()  { slet r = φF<πsmf>();  return r; }
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


φ<St> πφword()
{
  slet r = φcs("abcdefghjiklmnopqrstuvxyz", false, 1);
  return r;
}


φ<πname> πφname()
{
  slet r = φ2("name", φl("'"),
              φa("name",
                 φm(πφstr(),  [](St x) { return πname{x}; }),
                 φm(πφword(), [](St x) { return πname{x}; })));
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


πφ::πφ(φ<πf>                                  a0,
       F<φ<πf>        (φ<πf>, φ<πfs>)> const &es_,
       F<φ<πfs>       (φ<πf>, φ<πfs>)> const &ep_,
       F<φ<F<πf (πf)>>(φ<πf>, φ<πfs>)> const &os_,
       F<φ<F<πfs(πf)>>(φ<πf>, φ<πfs>)> const &op_)
{
  a = φa0<πf> ("πa"); auto &a_ = a.as<φa_<πf>> ();
  s = φa0<πf> ("πs"); auto &s_ = s.as<φa_<πf>> ();
  p = φa0<πfs>("πp"); auto &p_ = p.as<φa_<πfs>>();
  p = πφfs(φ1("πp₁", p, φo(φl(","))));

  // Important: if we don't weaken these, we get circular references which will
  // hold the parsers beyond this struct's demise
  let aw = φW(a);
  let sw = φW(s);
  let pw = φW(p);

  // These parsers are sufficiently weak and can be used directly
  os = os_(sw, pw);
  op = op_(sw, pw);
  es = es_(sw, pw);
  ep = ep_(sw, pw);

  let te = φ2("()", πφlp(), φm(pw, [](πfs &&f) -> πf
    { return [f=mo(f)](πi &i) -> πv
      { return {Sp<V<πv>>{new V<πv>{f(i)}}}; }; }), φo(πφrp()));

  a_ << te << πφgroup(sw) << πφwrap(a0);
  s_ << a
     << es
     << φm(φs("πφos", aw, os), [](let &x) -> πf { let &[a, f] = x; return f(a); });
  p_ << φm(sw, [](auto &&f)
           -> πfs { return [f=mo(f)](πi &i) { πvs r; r.push_back(f(i)); return r; }; })
     << ep
     << φm(φs("πφop", aw, op), [](let &x) -> πfs { let &[a, f] = x; return f(a); });
}


}

#include "end.hh"
