#include "pi.hh"

#include "begin.hh"

namespace τ
{


static φ<πfn> φop_;

φd_<πfn> &φopd()
{ if (!φop_) φop_.reset(new φd_<πfn>);
  return *dpc<φd_<πfn>>(φop_); }


static φ<πfn> binop(φ<πfn> p, St name, F<η0o(η, η)> f)
{
  return φm<πfn, πfn>(p, [=](πfn y)
    { πfn r;
      r += y;
      r << πinsn{name, [=](πi &i)
        { let r = i.dpop();
          let l = i.dpop();
          i.dpush(f(l.as_η(), r.as_η()));
          return πinsn_ok; }};
      return r; });
}


φ<πfn> φop()
{
  static bool i = false;
  if (!i)
  {
    φopd();
    i = true;
    φopd().def(
      "+", binop(φatom(), "+", [](η a, η b) { return η1o(a.pi() + b.pi()); }),
      "∷", binop(φatom(), "∷", [](η a, η b)
        { std::cerr << b.stv() << ": " << a << std::endl;
          return η1o(a); }));
  }
  return φop_;
}


}

#include "end.hh"
