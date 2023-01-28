#ifndef τΓ_h
#define τΓ_h


#include "types.hh"
#include "Xi.hh"
#include "gamma.hh"

#include "begin.hh"

namespace τ
{


// Base for γ compositions -- you can use this class directly, or you can
// create derived classes from it
struct Γ : public virtual γ
{
  virtual ~Γ() {}


  virtual Ξ operator()(Ξ &x)
    { Ξ y = x; for (let &g : gs) y = (*g)(y); return y; }

  Γ &operator<<(Sp<γ> const &x) { gs.push_back(x);  return *this; }
  Γ &operator>>(Sp<γ> const &x) { gs.push_front(x); return *this; }

  D<Sp<γ>>::iterator begin() { return gs.begin(); }
  D<Sp<γ>>::iterator end()   { return gs.end(); }


protected:
  D<Sp<γ>> gs;
};


}

#include "end.hh"


#endif
