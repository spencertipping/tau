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
  Φ             &f;
  πfs const     &fs;
  πh             g;  // global registers
  πs             d;  // data stack
  Sk<ζp, V<ζp>>  r;  // return stack

  πi(Φ &f_, πfs const &fs_, uN ng, ζp r_) : f(f_), fs(fs_), g(ng) { r.push(r_); }

  operator bool() const { return !r.empty(); }
  πr operator()()       { let i = i9{r.top()}; return fs[Sc<uN>(i[0])](*this, i); }
  πr go        ()       { while (*this) if (let r = (*this)()) return r; return 0; }
};


#if τdebug_iostream
O &operator<<(O &s, πi const &i)
{
  s << "πi nfs=" << i.fs.size() << " r=";
  if (i) s << i9{i.r.top()};
  else   s << "ω";
  return s << std::endl << i.g;
}
#endif


}

#include "end.hh"


#endif
