#ifndef tau_flux_h
#define tau_flux_h


// We don't depend on 32-bit boost::context, as that would create
// extra dev dependencies
#if tau_arch == tau_arch_linux64 \
    || tau_arch == tau_arch_wasm32 \
    || tau_arch == tau_arch_wasm64
# define tau_defines_flux 1

# include "flux/types.hh"
# include "flux/lambda.hh"
# include "flux/Lambda.hh"
# include "flux/theta.hh"
# include "flux/u9z.hh"
# include "flux/psi.hh"
# include "flux/phi.hh"
# include "flux/zeta.hh"
# include "flux/Gamma.hh"
# include "flux/gamma.hh"
#else
# define tau_defines_flux 0
# include "flux/types.hh"
#endif


#include "module/begin.hh"

namespace tau
{


using tau::flux::ψi;
using tau::flux::λi;
using tau::flux::φi;

using tau::flux::ζc0;


using tau::flux::Γ;
using tau::flux::γ;


}

#include "module/end.hh"


#endif
