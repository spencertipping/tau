#ifndef τπφ_h
#define τπφ_h

#include "types.hh"
#include "phi.hh"
#include "phi-ctor.hh"
#include "phi-auto.hh"
#include "pi-fn.hh"
#include "pi-phi-basic.hh"
#include "pi-phi-markers.hh"

#include "begin.hh"

namespace τ
{


struct πφP
{
  Tt auto p(πP<T>&&) const
    { return φm(p(std::declval<T>()), [](T &&x) { return πP<T>{x}; }); }
};

struct πφlit
{
  auto p(i64&&)   const { return πφint(); }
  auto p(f64&&)   const { return πφfloat(); }
  auto p(St&&)    const { return πφstr(); }
  auto p(ηname&&) const { return πφname(); }
};

struct πφstr
{
  template<chc *S> auto p(φaL<S>&&) const { return φl(St{S}); }
  template<chc *S> auto p(φaO<S>&&) const { return φo(φl(St{S})); }
  template<chc *S, bool N, u32 L, u32 U> auto p(φaCs<S, N, L, U>&&) const
    { return φcs(S, N, L, U); }
};


// An extensible π grammar. See pi-phi.md for details.
//
// To use this struct, inherit from it and structs like the above to add
// parsers.
struct πφ_
{
  πφ_() : s_  (φa0<πf<1>>("πs")),
          p_  (φa0<πf<1>>("πp")),
          ssp_(φd <πf<0>>("πssp")),
          psp_(φd <πf<0>>("πpsp")),
          spp_(φd <πf<0>>("πspp")),
          ppp_(φd <πf<0>>("πppp"))
    {
      // TODO: assemble toplevel parsers here
    }

  virtual ~πφ_() {}


  πφ_ &def_sa(φ<πf<1>> p) { s_.as<φa_<πf<1>>>() << p; return *this; }
  πφ_ &def_pa(φ<πf<1>> p) { p_.as<φa_<πf<1>>>() << p; return *this; }

  Tt πφ_ &def_ssp(St n, T f) { return def_(ssp_, n, f); }
  Tt πφ_ &def_psp(St n, T f) { return def_(psp_, n, f); }
  Tt πφ_ &def_spp(St n, T f) { return def_(spp_, n, f); }
  Tt πφ_ &def_ppp(St n, T f) { return def_(ppp_, n, f); }
  Tt πφ_ &def_sp (St n, T f) { return def_(sp_,  n, f); }
  Tt πφ_ &def_pp (St n, T f) { return def_(pp_,  n, f); }


protected:
  Tt πφ_ &def_(φ<πf<0>> &d, St n, T f)
    { let p = φauto(*this, πPsplit(n, std::function(f)));
      d.as<φd_<πf<0>>>().def(n, p);
      return *this; }

  φ<πf<1>> se_;   // singular expressions
  φ<πf<1>> pe_;   // plural expressions

  φ<πf<1>> s_;    // singular atoms (alt)
  φ<πf<1>> p_;    // plural atoms (alt)
  φ<πf<0>> ssp_;  // singular-operand, singular-return prefix (dispatch)
  φ<πf<0>> psp_;  // plural-operand, singular-return prefix (dispatch)
  φ<πf<0>> spp_;  // singular-operand, plural-return prefix (dispatch)
  φ<πf<0>> ppp_;  // plural-operand, plural-return prefix (dispatch)
  φ<πf<0>> sp_;   // singular → singular postfix (dispatch)
  φ<πf<0>> pp_;   // plural → plural postfix (dispatch)
};


struct πφ : public πφ_,
            public πφP,
            public πφlit,
            public πφstr {};


}

#include "end.hh"

#endif
