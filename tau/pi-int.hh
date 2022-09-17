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
  static constexpr uN ni = -1;

  Φ         &f;
  πp  const  p;  // bytecode program
  πh         g;  // global registers
  πs         d;  // data stack
  V<uN>      r;  // return stack
  uN         n;  // next instruction, or ni if none

  πi(Φ &f_, πp p_)
    : f(f_), p(p_), g(p.ng) { n = p.p0; }

  operator bool() const { return n != ni && !r.empty(); }
  πr operator()()       { return p(*this, n); }
  πr go        ()       { while (*this) if (let r = (*this)()) return r; return 0; }
};


#if τdebug_iostream
O &operator<<(O &s, πi const &i)
{
  s << "πi r=";
  if (i) s << i.r.back();
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
