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


M<Γs, λf> Γfs;


// TODO: how do we register an on-exit handler? we need one to delete g
// within this lambda; otherwise we can't set an alternate return.
#define Γ(name, ...)                                            \
  (Γfs[name] = [](γ &g, i9 Ξ, γφ &i, γφ &o, γφ &c, γφ &d)       \
    { __VA_ARGS__;                                              \
      return 0; })


}

#include "end.hh"


#endif
