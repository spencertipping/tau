#ifndef τdebug_h
#define τdebug_h


#include "arch.hh"


#if !defined(τdebug)
# define τdebug 1
#endif

#if !defined(τallow_todo)
# define τallow_todo τdebug
#endif

#if !defined(τtrack_ψ)
# define τtrack_ψ τdebug
#endif

#if !defined(τtrack_ξ)
# define τtrack_ξ τdebug
#endif

#if τdebug
# include <iostream>
#endif


#if !defined(τdebug_catchall)
# define τdebug_catchall τdebug
#endif


#if τhas_stacktrace
# include <boost/stacktrace.hpp>
#else
  namespace boost::stacktrace
  {
  inline std::string stacktrace() { return ""; }
  }
#endif


[[noreturn]] inline void τunreachable()
{
  std::cerr << "τunreachable() called" << std::endl;
  std::exit(1);
}


#if τhas_fast_exceptions
# include <errno.h>
# include <iostream>
# include <sstream>

# define τassert_begin try {
# define τassert_end   } catch (std::string const e)          \
                         { std::cerr << "FAIL: "              \
                                     << e << std::endl; }

# define τassert_fail(x, f, l, m)                                       \
  ([&]() {                                                              \
    static bool τdebugging = false;                                     \
    std::stringstream τafs;                                             \
    if (!τdebugging)                                                    \
    {                                                                   \
      τdebugging = true;                                                \
      τafs << m << ", errno = " << strerror(errno)                      \
           << "\nat " << boost::stacktrace::stacktrace() << "\n";       \
    }                                                                   \
    throw τafs.str();                                                   \
  })()

#elif τhas_assert_fail  // if τhas_fast_exceptions

# define τassert_begin
# define τassert_end

# include <errno.h>
# define τassert_fail(x, f, l, m)                                       \
  ([&]() {                                                              \
    static bool τdebugging = false;                                     \
    if (!τdebugging)                                                    \
    {                                                                   \
      τdebugging = true;                                                \
      if (errno)                                                        \
        perror("assertion failure errno (if any)");                     \
      std::cerr << "FAIL: " << m << "\nat "                             \
                << boost::stacktrace::stacktrace() << std::endl;        \
    }                                                                   \
    __assert_fail(x, f, l, __ASSERT_FUNCTION);                          \
    τunreachable();                                                     \
  })()

#else    // elif τhas_assert_fail

# define τassert_begin
# define τassert_end
# define τassert_fail(x, f, l, m)                                    \
  ([&]() {                                                           \
    static bool τdebugging = false;                                  \
    if (!τdebugging)                                                 \
    {                                                                \
      τdebugging = true;                                             \
      if (errno)                                                     \
        perror("assertion failure errno (if any)");                  \
      std::cerr << "FAIL: " << m << "\nat "                          \
                << boost::stacktrace::stacktrace() << std::endl;     \
      std::cerr << "NOTE: assertion location may be wrong"           \
                << std::endl;                                        \
    }                                                                \
    assert(0);                                                       \
    τunreachable();                                                  \
  })()

#endif   // if τhas_fast_exceptions ... elif has_assert_fail ... else


#endif
