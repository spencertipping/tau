#ifndef tau_debug_h
#define tau_debug_h


#if !defined(tau_debug)
# define tau_debug 1
#endif


#if !defined(tau_debug_nop)
# define tau_debug_nop (!tau_debug)
#endif


#if tau_debug
# include <iostream>
#endif


#if !defined(tau_debug_iostream)
# define tau_debug_iostream tau_debug
#endif


#if tau_dangerous_disable_asserts

# warning assertions are disabled
# undef assert
// NOTE: assert() may produce side-effects, so we need to keep those
// even if not debugging
# define assert(x) (x)

#else
# include <cassert>
#endif


#endif
