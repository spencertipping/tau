#ifdef τbegin_h
# error already inside a module
#endif

#define τbegin_h


#if τdangerous_disable_asserts

# warning assertions are disabled
# undef assert
// NOTE: assert() may produce side-effects, so we need to keep those
// even if not debugging
# define assert(x) void(static_cast<bool>(x))
# define A(x, m)   assert(x)
# define TA(x, m)  default: return x;

#else
# include <cassert>
# define A(x, m)                                          \
    (static_cast<bool>(x)                                 \
     ? void(0)                                            \
     : τassert_fail(#x, __FILE__, __LINE__, m))

# define TA(r, m) default: A(0, m); return r;
#endif


#define AE(x, y) \
  A((x) == (y), \
    #x "(" << (x) << ") != " #y "(" << (y) << ")");


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

#define Sc  static_cast
#define Rc  reinterpret_cast
#define Cc  const_cast
#define Dc  dynamic_cast
#define Duc std::chrono::duration_cast

#define Tn   template<>
#define Tt   template<class T>
#define Tx   template<class X>
#define Txy  template<class X, class Y>
#define Txs  template<class... Xs>
#define Txxs template<class X, class... Xs>
