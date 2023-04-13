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


φ<πf>  πφcore_a() { return φm(πφlit(), πvq); }
φ<πmf> πφcore_m()
{
  return φd<πmf>("sin",   πηsin(),
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

                 "~",  πηinv(),
                 "-",  πηneg(),
                 "!",  πηnot(),
                 "!!", πηnotnot());
}

φ<πdf> πφcore_d()
{
  return φd<πdf>("+",  πηadd(),
                 "-",  πηsub(),
                 "**", πηpow(),
                 "*",  πηmul(),
                 "//", πηdiv(),
                 "<<", πηlsh(),
                 ">>", πηrsh(),
                 "%",  πηmod(),
                 "&",  πηand(),
                 "|",  πηor(),
                 "^",  πηxor(),
                 "<=", πηle(),
                 ">=", πηge(),
                 "<>", πηcmp(),
                 "<",  πηlt(),
                 ">",  πηgt(),
                 "=",  πηeq(),
                 "!=", πηne());
}


φ<πtf> πφcore_t()
{
  return φd<πtf>("?", πηcond());
}


φ<St> πφws() { return φcs(" \t\n\r", false, 1); }
φ<St> πφlc() { return φq(φs(φl("# ", ""), φcs("\n", true))); }
φ<St> πφig() { return φq(φn(φa<St>(πφws(), πφlc()))); }


φ<i64> πφint()     { return φa(πφint_hex(), πφint_bin(), πφint_oct(), πφint_dec()); }
φ<i64> πφint_dec() { return φm(φq(φo(φl("-")),           φcs("0123456789",             false, 1)), [](St x) -> i64 { return std::stoll(x, nullptr, 10); }); }
φ<i64> πφint_oct() { return φm(φq(φo(φl("-")), φl("0"),  φcs("01234567",               false, 1)), [](St x) -> i64 { return std::stoll(x, nullptr, 8);  }); }
φ<i64> πφint_bin() { return φm(φ2(             φl("0b"), φcs("01",                     false, 1)), [](St x) -> i64 { return std::stoll(x, nullptr, 2);  }); }
φ<i64> πφint_hex() { return φm(φq(φo(φl("-")), φl("0x"), φcs("0123456789abcdefABCDEF", false, 1)), [](St x) -> i64 { return std::stoll(x, nullptr, 16); }); }

φ<f64> πφfloat()
{
  let ds  = φcs("0123456789", false, 1);
  let exp = φq(φcs("eE", false, 1, 1), φcs("-+", false, 0, 1), ds);
  return φm(φq(φo(φl("-")), φa(φq(ds, φl("."), φo(φs(ds, φo(exp)))),
                               φq(    φl("."),    φs(ds, φo(exp))),
                               φq(ds,                       exp))),
            [](St x) { return std::stod(x); });
}


φ<πname> πφname() { return φm(φ2(φl("'"), φcs("abcdefghjiklmnopqrstuvxyz", false, 1)), [](St x) { return πname{x}; }); }

φ<St> πφstr_escape()
{
  return φa(φl("\\"),
            φl("\""),
            φl<St>("n", "\n"),
            φl<St>("r", "\r"),
            φl<St>("t", "\t"));
}

φ<St> πφstr()
{
  return φm(φ2(φl("\""),
               φn(φa(φcs("\\\"", true, 1), φ2(φl("\\"), πφstr_escape()))),
               φl("\"")),
            [](V<St> xs)
              { let n = std::accumulate(xs.begin(), xs.end(), 0, [](i64 a, Stc &b) { return a + b.size(); });
                St  r; r.reserve(n);
                for (let &x : xs) r.append(x);
                return r; });
}

φ<πv> πφlit()
{
  return φa<πv>(φm(πφfloat(), [](f64 x)   { return πv{x}; }),
                φm(πφint(),   [](i64 x)   { return πv{x}; }),
                φm(πφname(),  [](πname x) { return πv{x}; }),
                φm(πφstr(),   [](St x)    { return πv{x}; }));
}


φ<πf> πφtuple_expr(φ<πf> p)
{
  return φm(πφtuple(p), [](Vc<πf> &xs) -> πf
    { return [xs](πi &i) -> πv
      { let ys = new V<πv>; ys->reserve(xs.size());
        for (let &x : xs) ys->push_back(x(i));
        return πv{Sp<V<πv>>(ys)}; }; });
}


φ<πf> πφ(φ<πf> a, φ<πmf> m, φ<πdf> d, φ<πtf> t)
{
  let p1 = φa0<πf>();  auto &p1a = p1.as<φa_<πf>>();
  let p  = φa0<πf>();  auto &pa  = p .as<φa_<πf>>();
  let pr = φN("π", p);

  p1a << πφgroup(pr) << πφtuple_expr(pr) << πφwrap(a);
  pa  << φm(φs(πφwrap(m), pr),         [](auto r) { let &[m, x]       = r; return πmc(m, x); })
      << φm(φs(p1, πφwrap(d), pr),     [](auto r) { let &[x, d, y]    = r; return πdc(d, x, y); })
      << φm(φs(p1, πφwrap(t), pr, pr), [](auto r) { let &[x, t, y, z] = r; return πtc(t, x, y, z); })
      << p1;

  return p;
}


}

#include "end.hh"
