#ifndef tau_flux_h
#define tau_flux_h


// We don't depend on 32-bit boost::context, as that would create
// extra dev dependencies
#if tau_arch == tau_arch_linux64 \
    || tau_arch == tau_arch_wasm32 \
    || tau_arch == tau_arch_wasm64


# define tau_defines_flux 1

# include "flux/types.hh"
# include "flux/lambda.hh"
# include "flux/Lambda.hh"
# include "flux/theta.hh"
# include "flux/u9z.hh"
# include "flux/psi.hh"
# include "flux/phi.hh"
# include "flux/zeta.hh"
# include "flux/Gamma.hh"
# include "flux/gamma.hh"


# include "module/begin.hh"

  namespace tau
  {


  using tau::flux::ψi;
  using tau::flux::λi;
  using tau::flux::φi;
  using tau::flux::γi;
  using tau::flux::γsi;

  using tau::flux::ζc0;

  using tau::flux::Γ;
  using tau::flux::γ;


  inline constexpr γsi operator""_q(char const *s, uN l) { return flux::str_γsi(s, l); }
  inline constexpr γsi operator""_l(char const *s, uN l) { return operator""_q(s, l); }


  // NOTE: symbol identity is really important, and some compilers
  // do weird things
  static_assert(""_q != "a"_q);
  static_assert("a"_q != "aa"_q);
  static_assert("aa"_q != "aaa"_q);
  static_assert("aaa"_q != "aaaa"_q);
  static_assert("aaaa"_q != "aaaaa"_q);
  static_assert("aaaaa"_q != "aaaaaa"_q);
  static_assert("aaaaaa"_q != "aaaaaaa"_q);
  static_assert("aaaaaaa"_q != "aaaaaaaa"_q);
  static_assert("aaaaaaaa"_q != "aaaaaaab"_q);
  static_assert("aaaaaaaa"_q != "baaaaaaa"_q);


  }

# include "module/end.hh"


#else
# define tau_defines_flux 0
# include "flux/types.hh"
#endif


#endif
