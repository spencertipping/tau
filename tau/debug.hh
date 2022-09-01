#ifndef tau_debug_h
#define tau_debug_h


#include "arch.hh"


#if !defined(tau_debug)
# define tau_debug 1
#endif


#if tau_debug
# include <iostream>
#endif


#if !defined(tau_debug_iostream)
# define tau_debug_iostream tau_debug
#endif


#if !defined tau_debug_i9st
# define tau_debug_i9st 0
#endif


#if tau_has_assert_fail
# include <errno.h>
# if tau_debug_iostream
#   define tau_assert_fail(x, f, l, m)                      \
      ([&]() {                                              \
        static bool tau_debugging = false;                  \
        if (!tau_debugging)                                 \
        {                                                   \
          tau_debugging = true;                             \
          if (errno)                                        \
            perror("assertion failure errno (if any)");     \
          std::cerr << "FAIL: " << m << std::endl;          \
        }                                                   \
        __assert_fail(x, f, l, __ASSERT_FUNCTION);          \
      })()
# else
#   define tau_assert_fail(x, f, l, m)            \
      __assert_fail(x, f, l, __ASSERT_FUNCTION)
# endif
#endif


#endif
