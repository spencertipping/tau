#ifndef tau_debug_h
#define tau_debug_h


#if !defined(tau_debug)
# define tau_debug 1
#endif


#if tau_debug
# include <iostream>
#endif


#if !defined(tau_debug_iostream)
# define tau_debug_iostream tau_debug
#endif


#if tau_debug_iostream
# define tau_assert_fail(x, f, l, m)                  \
  ([&]() {                                            \
    std::cerr << "FAIL: " << m << std::endl;          \
    __assert_fail(x, f, l, __ASSERT_FUNCTION);        \
  })()
#else
# define tau_assert_fail(x, f, l, m)            \
  __assert_fail(x, f, l, __ASSERT_FUNCTION)
#endif


#endif
