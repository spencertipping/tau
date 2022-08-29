#ifndef tau_h
#define tau_h


#include "tau/debug.hh"
#include "tau/arch.hh"

#define tau_defines_λ                            \
   (   tau_arch == tau_arch_linux64              \
    || tau_arch == tau_arch_wasm32               \
    || tau_arch == tau_arch_wasm64)

#if tau_defines_λ
# include "tau/lambda.hh"
# include "tau/Lambda.hh"
# include "tau/zeta.hh"
# include "tau/phi.hh"
# include "tau/gamma.hh"
#endif


#include "tau/utf9.hh"
#include "tau/numerics.hh"
#include "tau/shd.hh"


#endif
