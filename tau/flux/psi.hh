#ifndef tau_flux_psi_h
#define tau_flux_psi_h


#include "../types.hh"

#include "types.hh"
#include "zeta.hh"
#include "lambda.hh"
#include "Lambda.hh"


#include "../module/begin.hh"

namespace tau::flux
{


struct Ψ
{
  struct φ
  { ψi    a,  b;
    ζ<ζv> ab, ba; };

  Λ         &l;
  uN         c_;
  M<φi, φ>   cs;
  M<ψi, φi>  qs;
  φi         ci{0};
  ψi         qi{0};

  Ψ(Λ &l_, uN c__ = 64) : l(l_), c_(c__) {}

  ψi ψc() { qs[ιi(qi, qs)] = 0; return qi; }

  φi φc(ψi a, ψi b, uN c = 0)
    { assert(qs.at(a) == 0 && qs.at(b) == 0);
      if (!c) c = c_;
      cs[qs[a] = qs[b] = ιi(ci, cs)] = {a, b, ζ<ζv>(c), ζ<ζv>(c)};
      return ci; }

  Ψ &φx(φi i)
    { let &c = cs[i];
      qs.erase(c.a); qs.erase(c.b);
      cs.erase(i);
      return *this; }
};


#if tau_debug_iostream
O &operator<<(O &s, Ψ const &q)
{
  return s << "TODO: Ψ";
}
#endif


}

#include "../module/end.hh"


#endif
