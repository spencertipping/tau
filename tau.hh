#ifndef τ_h
#define τ_h


#include "tau/debug.hh"
#include "tau/arch.hh"


#define τdefines_λ (τhas_boost_context || τhas_emscripten_fiber)


#include "tau/numerics.hh"
#include "tau/shd.hh"


#if τdefines_λ

# include "tau/lambda.hh"
# include "tau/Lambda.hh"
# include "tau/zeta.hh"
# include "tau/phi.hh"
# include "tau/utf9.hh"

# include "tau/Phi.hh"

# include "tau/pi0.hh"
# include "tau/pi1.hh"
# include "tau/pi2.hh"

# include "tau/gamma.hh"
# include "tau/digamma.hh"
# include "tau/Gamma.hh"
# include "tau/Xi.hh"

# include "tau/xi.hh"

#else
# warning λ unsupported on this platform; almost nothing will work
#endif


#endif
