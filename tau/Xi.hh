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

  ξi i(Stc &k, uN c) { return at(k, c).i(); }
  ξo o(Stc &k, uN c) { return at(k, c).o(); }

  ξio &at(Stc &k, uN c)
    { if (!xs.contains(k)) xs[k] = ξio{ms(ξ(l, c))};
      return xs.at(k); }


protected:
  Λ          &l;
  M<St, ξio>  xs;
};


}

#include "end.hh"


#endif
