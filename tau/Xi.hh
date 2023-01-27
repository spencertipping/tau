#ifndef τΞ_h
#define τΞ_h


#include "Lambda.hh"
#include "xiio.hh"
#include "begin.hh"

namespace τ
{


struct Ξ
{
  Ξ(Λ &l_) : l(l_) {}

  ξi i(Stc &k, uN c) { return at(k, c)->i(); }
  ξo o(Stc &k, uN c) { return at(k, c)->o(); }

  Sp<ξio> &at(Stc &k, uN c)
    { if (!xs.contains(k)) xs[k].reset(new ξio(l, c));
      else
      { let c0 = xs.at(k)->capacity();
        A(c <= c0, "ξ capacity mismatch: " << c << " > " << c0); }
      return xs.at(k); }


protected:
  Λ              &l;
  M<St, Sp<ξio>>  xs;
};


}

#include "end.hh"


#endif
