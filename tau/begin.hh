#ifdef τbegin_h
# error already inside a module
#endif

#define τbegin_h


#if τdangerous_disable_asserts

# warning assertions are disabled
# undef assert
// NOTE: assert() may produce side-effects, so we need to keep those
// even if not debugging
# define assert(x) (x)
# define A(x, m) assert(x)
# define TA(x, m)

#else
# include <cassert>
# if τhas_assert_fail
#   define A(x, m)                                          \
      (static_cast<bool>(x)                                 \
       ? void(0)                                            \
       : τassert_fail(#x, __FILE__, __LINE__, m))
# else
#   define A(x, m) assert(x)
# endif

# if τdebug_catchall
#   define TA(r, m) default: A(0, m); return r;
# else
#   define TA(r, m)
# endif
#endif


#if τallow_todo
# define TODO(x) A(0, "TODO: " << x)
#else
# define TODO(x) TODO < #x >
#endif


#define let   auto const
#define letc  let constexpr
#define slet  static let
#define sletc static letc

#define ic inline constexpr
#define sc static constexpr

#define Sc static_cast
#define Rc reinterpret_cast
#define Cc const_cast


#define defR(I)                                                         \
  template<class J, class T>                                            \
  ic typename std::enable_if<std::is_same<I, J>::value, I>::type        \
  R(T xs, uN i)

#define defRI(s)                                                        \
  template<class I, class T>                                            \
  ic typename std::enable_if<std::is_integral<I>::value                 \
                             and sizeof(I) == s, I>::type               \
  R(T xs, uN i)

#define defW(I)                                                         \
  template<class J, class T>                                            \
  ic typename std::enable_if<std::is_same<I, J>::value, void>::type     \
  W(T xs, uN i, I x)

#define defWI(s)                                                        \
  template<class I, class T>                                            \
  ic typename std::enable_if<std::is_integral<I>::value                 \
                             and sizeof(I) == s, void>::type            \
  W(T xs, uN i, I x)
