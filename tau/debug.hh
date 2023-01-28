#ifndef τdebug_h
#define τdebug_h


#include "arch.hh"


#if !defined(τdebug)
# define τdebug 1
#endif


#if !defined(τallow_todo)
# define τallow_todo τdebug
#endif


#if τdebug
# include <iostream>
#endif


#if !defined(τdebug_i9st)
# define τdebug_i9st 0
#endif


#if !defined(τπ0debug_bounds_checks)
# define τπ0debug_bounds_checks τdebug
#endif


#if !defined(τdebug_catchall)
# define τdebug_catchall τdebug
#endif


#if τhas_fast_exceptions
# include <errno.h>
# include <iostream>
# include <sstream>

# define τassert_begin try {
# define τassert_end   } catch (std::string const e)          \
                         { std::cerr << "FAIL: "              \
                                     << e << std::endl; }

# define τassert_fail(x, f, l, m)                               \
  ([&]() {                                                      \
    static bool τdebugging = false;                             \
    std::stringstream τafs;                                     \
    if (!τdebugging)                                            \
    {                                                           \
      τdebugging = true;                                        \
      τafs << m << ", errno = " << strerror(errno);             \
    }                                                           \
    throw τafs.str();                                           \
  })()

#elif τhas_assert_fail  // if τhas_fast_exceptions

# define τassert_begin
# define τassert_end

# include <errno.h>
# define τassert_fail(x, f, l, m)                       \
  ([&]() {                                              \
    static bool τdebugging = false;                     \
    if (!τdebugging)                                    \
    {                                                   \
      τdebugging = true;                                \
      if (errno)                                        \
        perror("assertion failure errno (if any)");     \
      std::cerr << "FAIL: " << m << std::endl;          \
    }                                                   \
    __assert_fail(x, f, l, __ASSERT_FUNCTION);          \
  })()

#else    // elif τhas_assert_fail

# define τassert_begin
# define τassert_end
# define τassert_fail(x, f, l, m)                          \
  ([&]() {                                                 \
    static bool τdebugging = false;                        \
    if (!τdebugging)                                       \
    {                                                      \
      τdebugging = true;                                   \
      if (errno)                                           \
        perror("assertion failure errno (if any)");        \
      std::cerr << "FAIL: " << m << std::endl;             \
      std::cerr << "NOTE: assertion location may be wrong" \
                << std::endl;                              \
    }                                                      \
    assert(0);                                             \
  })()

#endif   // if τhas_fast_exceptions ... elif has_assert_fail ... else


#endif
