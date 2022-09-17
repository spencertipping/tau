#ifndef τ_h
#define τ_h


#include "tau/debug.hh"
#include "tau/arch.hh"


#define τdefines_λ (τhas_boost_context || τhas_emscripten_fiber)
#define τdefines_Φ (τdefines_λ && τhas_epoll)
#define τdefines_π (τdefines_Φ)
#define τdefines_γ (τdefines_Φ)
#define τdefines_ξ (τdefines_γ)


#include "tau/numerics.hh"
#include "tau/shd.hh"


// TODO: unify all of these things down to just λ once we have
// epoll -> emscripten portability (via Φ alternatives)

#if τdefines_λ
# include "tau/lambda.hh"
# include "tau/Lambda.hh"
# include "tau/zeta.hh"
# include "tau/phi.hh"
# include "tau/utf9.hh"
#else
# warning λ unsupported on this platform; almost nothing will work
#endif


#if τdefines_Φ
# include "tau/Phi.hh"
#endif


#if τdefines_π
# include "tau/pi.hh"
#endif


#if τdefines_γ
# include "tau/gamma.hh"
# include "tau/digamma.hh"
# include "tau/Gamma.hh"
# include "tau/Xi.hh"
#endif


#if τdefines_ξ
# include "tau/xi.hh"
#endif


#endif
