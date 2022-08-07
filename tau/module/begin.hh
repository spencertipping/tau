#ifdef tau_begin_h
# error already inside a module
#endif

#define tau_begin_h


#ifndef tau_debug
# define tau_debug 1
#endif


#ifndef tau_debug_iostream
# define tau_debug_iostream tau_debug
#endif


#if tau_debug_iostream
# include <iostream>
#endif


#if tau_debug
# include <cassert>
#else
# define assert(x) void(0)
#endif


#define let auto const
