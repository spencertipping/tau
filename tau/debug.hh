#ifndef τdebug_h
#define τdebug_h


#include "arch.hh"


#if !defined(τdebug)
# define τdebug 1
#endif


#if τdebug
# include <iostream>
#endif


#if !defined(τdebug_iostream)
# define τdebug_iostream τdebug
#endif


#if !defined τdebug_i9st
# define τdebug_i9st 0
#endif


#if !defined(τπ0debug_bounds_checks)
# define τπ0debug_bounds_checks τdebug
#endif


#if τhas_assert_fail
# include <errno.h>
# if τdebug_iostream
#   define τassert_fail(x, f, l, m)                         \
  ([&]() {                                                  \
    static bool τdebugging = false;                         \
    if (!τdebugging)                                        \
    {                                                       \
      τdebugging = true;                                    \
      if (errno)                                            \
        perror("assertion failure errno (if any)");         \
      std::cerr << "FAIL: " << m << std::endl;              \
    }                                                       \
    __assert_fail(x, f, l, __ASSERT_FUNCTION);              \
  })()
# else
#   define τassert_fail(x, f, l, m)             \
  __assert_fail(x, f, l, __ASSERT_FUNCTION)
# endif
#endif


#endif
