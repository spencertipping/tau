#ifndef τπint_h
#define τπint_h


#include "types.hh"
#include "numerics.hh"
#include "Phi.hh"
#include "Lambda.hh"
#include "zeta.hh"
#include "utf9.hh"

#include "pi-types.hh"
#include "pi-gc.hh"


#include "begin.hh"

namespace τ
{


struct πi
{
  sc u32 ni = -1;

  Φ        &f;
  πp const  p;  // bytecode program
  πh        g;  // global registers
  πs        d;  // data stack
  V<u32>    r;  // return stack
  u32       n;  // next instruction, or ni if none

  πi(Φ &f_, πp p_) : f(f_), p(p_), g(p.ng) { n = p.p0; }

  πb operator [](uN i) const { return p.b[i]; }
  operator  bool()     const { return n != ni || !r.empty(); }
  πi &operator()()           { p(*this, n);             return *this; }
  πi &go        ()           { while (*this) (*this)(); return *this; }
};


#if τdebug_iostream
O &operator<<(O &s, πi const &i)
{
  s << "πi n=";
  if (i) s << i.n;
  else   s << "ω";
  return s << std::endl
           << " p=" << i.p
           << " d=" << i.d
           << " g=" << i.g;
}
#endif


}

#include "end.hh"


#endif
