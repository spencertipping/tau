#ifndef tau_h
#define tau_h


#include "tau/debug.hh"
#include "tau/arch.hh"


#define tau_defines_λ (tau_has_boost_context || tau_has_emscripten_fiber)
#define tau_defines_Φ (tau_defines_λ && tau_has_epoll)
#define tau_defines_γ (tau_defines_Φ)


#include "tau/numerics.hh"
#include "tau/shd.hh"


// TODO: unify all of these things down to just λ once we have
// epoll -> emscripten portability (via Φ alternatives)

#if tau_defines_λ
# include "tau/lambda.hh"
# include "tau/Lambda.hh"
# include "tau/zeta.hh"
# include "tau/phi.hh"
# include "tau/utf9.hh"
#else
# warning λ unsupported on this platform; almost nothing will work
#endif


#if tau_defines_Φ
# include "tau/Phi.hh"
#endif


#if tau_defines_γ
# include "tau/gamma.hh"
#endif


#endif
