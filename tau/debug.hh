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


#if defined(tau_debug_assert) ? tau_debug_assert : tau_debug
# include <cassert>
#else
# undef assert
# define assert(x) void(0)
#endif


#endif
