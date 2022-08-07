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

  Λ                &l;
  M<φi, φ>          cs;
  M<ψi, P<φi, φi>>  qs;
  φi                ci{0};
  ψi                qi{0};

  Ψ(Λ &l_) : l(l_) {}

  // TODO: frame the API for making connections and issuing IO
  ψi ψc() { let i = ιi(qi, qs); qs[i] = std::make_pair(0, 0); return i; }

  φi φc(ψi a, ψi b)
    { // TODO: assert no existing connection
      let i = ιi(ci, cs);

    }
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
