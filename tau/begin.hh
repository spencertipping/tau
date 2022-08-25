#ifdef tau_begin_h
# error already inside a module
#endif

#define tau_begin_h


#if tau_dangerous_disable_asserts

# warning assertions are disabled
# undef assert
// NOTE: assert() may produce side-effects, so we need to keep those
// even if not debugging
# define assert(x) (x)
# define A(x, m) assert(x)
# define TA(x)

#else
# include <cassert>
# define A(x, m)                                        \
  (static_cast<bool>(x)                                 \
   ? void(0)                                            \
   : tau_assert_fail(#x, __FILE__, __LINE__, m))
# define TA(r) default: A(0, "unreachable case"); return r;
#endif


#define let auto const
