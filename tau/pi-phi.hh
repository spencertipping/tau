#ifndef τπφ_h
#define τπφ_h

#include "types.hh"
#include "phi.hh"
#include "phi-ctor.hh"
#include "phi-auto.hh"
#include "pi-auto.hh"
#include "pi-fn.hh"
#include "pi-phi-basic.hh"
#include "pi-phi-markers.hh"

#include "begin.hh"

namespace τ
{


// An extensible π grammar. See pi-phi.md for details.
//
// To use this struct, inherit from it for the core grammar and mix in
// structs from pi-phi-basic.hh to add parsers. For example,
// struct πφ below.
struct πφ_
{
  πφ_();
  virtual ~πφ_() {}


  static π1 pre (π0 a, π1 b) { return b | a; }
  static π1 post(π1 a, V<π0> const &b)
    { π1 r = a; r << b; return r; }


  πφ_ &def_sa(φ<π1> p) { s_.as<φa_<π1>>() << p; return *this; }
  πφ_ &def_pa(φ<π1> p) { p_.as<φa_<π1>>() << p; return *this; }

  Tt πφ_ &def_ssp(St n, T f) { return def_(ssp_, n, f); }
  Tt πφ_ &def_psp(St n, T f) { return def_(psp_, n, f); }
  Tt πφ_ &def_spp(St n, T f) { return def_(spp_, n, f); }
  Tt πφ_ &def_ppp(St n, T f) { return def_(ppp_, n, f); }
  Tt πφ_ &def_sp (St n, T f) { return def_(sp_,  n, f); }
  Tt πφ_ &def_pp (St n, T f) { return def_(pp_,  n, f); }

  // NOTE: returning the wrong type is intentional. Every πsa<T> should
  // arise from a π1 that we parse, and it's up to πauto to convert the
  // π1 to T using immediate stack indirection.
  Tt φ<πsa<π1>> p(πsa<T> const&) const { return φm(s_,  [](π1 &&x) { return πsa<π1>{mo(x)}; }); }
  Tt φ<πpa<π1>> p(πpa<T> const&) const { return φm(p_,  [](π1 &&x) { return πpa<π1>{mo(x)}; }); }
  Tt φ<πse<π1>> p(πse<T> const&) const { return φm(se_, [](π1 &&x) { return πse<π1>{mo(x)}; }); }
  Tt φ<πpe<π1>> p(πpe<T> const&) const { return φm(pe_, [](π1 &&x) { return πpe<π1>{mo(x)}; }); }


protected:
  Tt πφ_ &def_(φ<π0> &d, Stc &n, T f)
    { d.as<φd_<π0>>().def(n, πauto(*this, n, std::function(f)));
      return *this; }

  φ<π1> se_;   // singular expressions
  φ<π1> pe_;   // plural expressions

  φ<π1> s_;    // singular atoms (alt)
  φ<π1> p_;    // plural atoms (alt)

  φ<π0> ssp_;  // singular-operand, singular-return prefix (dispatch)
  φ<π0> psp_;  // plural-operand, singular-return prefix (dispatch)
  φ<π0> spp_;  // singular-operand, plural-return prefix (dispatch)
  φ<π0> ppp_;  // plural-operand, plural-return prefix (dispatch)
  φ<π0> sp_;   // singular → singular postfix (dispatch)
  φ<π0> pp_;   // plural → plural postfix (dispatch)
};


// Example instantiation of πφ_
struct πφ : public πφ_,
            public πφP,
            public πφlit,
            public πφstr {};


}

#include "end.hh"

#endif
