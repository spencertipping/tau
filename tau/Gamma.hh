#ifndef tau_Γ_h
#define tau_Γ_h


#include "types.hh"
#include "gamma.hh"

#include "begin.hh"

namespace tau
{


enum class Γs
{
  iota,
  delay,
  fd_in,
  fd_out,
};


// TODO: define Γf type
typedef F<Λr()> Γf;


M<Γs, λf> Γfs;


#define Γ(name) Γfs[name] = [](γ &g, i9 Ξ, γφ &ι, γφ &ο, γφ &ξ, γφ &δ)


}

#include "end.hh"


#endif
