#include <algorithm>

#include "auto.hh"
#include "psi2.hh"
#include "begin.hh"

namespace σ
{

using namespace τ;


φ<Ψ2> &φΨ2()
{
  static φ<Ψ2> r = φL<Ψ2>([]()
    { return φd<Ψ2>("Ψ2",
                    "g",  φauto(Γg),
                    "-",  φauto(Γid),
                    "M",  φauto(ΓM)
                    /*"j",  φauto(Γj),
                      "k",  φauto(Γk),
                      "p",  φauto(Γp),
                      "rp", φauto(Γrp),
                      "r-", φauto(Γrd),
                      "r",  φauto(Γrt),
                      #if τlinux
                      "h",  φauto(Γh),
                      "s",  φauto(Γsshc),
                      "t",  φauto(Γtcpc),
                      "u",  φauto(Γudpc),
                      "v",  φauto(Γunixc),
                      #endif
                      "w",  φauto(Γw),
                      "W",  φauto(ΓW),
                      "x",  φauto(Γx),
                      "y",  φauto(Γy),
                      "J",  φauto(ΓJ),
                      "N",  φauto(ΓN),
                      "+",  φauto(Γappend),
                      "^",  φauto(Γprepend),
                      "%",  φauto(Γunion)*/); });
  return r;
}


Ψ2 ΓM(St name)
{
  return [=](ψ q, ξi i, ξo o, Ψaux)
    {
      ηl(i, o, [&](ηi x)
        { std::cout << name << ": " << x << std::endl;
          o << x; });
    };
}


Ψ2 Γid()
{
  return [=](ψ, ξi i, ξo o, Ψaux) { for (let x : i) o << x; };
}


Ψ2 Γg(πf f)
{
  return [=](ψ q, ξi i, ξo o, Ψaux)
    {
      V<B> xs;
      ητ(i, o,
         [&](ηi x) { xs.push_back(B{x.odata(), x.osize()}); },
         [&]()
           { // TODO: sort by f(x), not x itself
             // FIXME: we need ηi ordering, not B ordering
             // TODO: create η container
             std::sort(xs.begin(), xs.end());
             for (let &x : xs) o.r(x.size()) << Sn<u8c>{x.data(), x.size()};
             xs.clear();
             o.r() << ηsig::τ; });
    };
}


}

#include "end.hh"
