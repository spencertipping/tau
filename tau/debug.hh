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


#if !defined(τdebug_iostream)
# define τdebug_iostream τdebug
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
# if τdebug_iostream
#   include <errno.h>
#   include <iostream>
#   include <sstream>

#   define τassert_begin try {
#   define τassert_end   } catch (std::string const e)          \
                           { std::cerr << "FAIL: "              \
                                       << e << std::endl; }

#   define τassert_fail(x, f, l, m)                             \
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
# else
#   define τassert_fail(x, f, l, m)             \
  __assert_fail(x, f, l, __ASSERT_FUNCTION)
# endif                 // if τdebug_iostream

#elif τhas_assert_fail  // if τhas_fast_exceptions

# define τassert_begin
# define τassert_end

# if τdebug_iostream
#   include <errno.h>
#   define τassert_fail(x, f, l, m)                     \
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
# else
#   define τassert_fail(x, f, l, m)             \
  __assert_fail(x, f, l, __ASSERT_FUNCTION)
# endif  // if τdebug_iostream

#else    // elif τhas_assert_fail

# define τassert_begin
# define τassert_end
# if τdebug_iostream
#   define τassert_fail(x, f, l, m)                        \
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
# else
#   define τassert_fail(x, f, l, m) assert(0)
# endif  // if τdebug_iostream

#endif   // if τhas_fast_exceptions ... elif has_assert_fail ... else


#endif
