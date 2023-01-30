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
struct Γ
{
  Γ() {}
  Γ(Sp<γ> x)    { *this << x; }
  Γ(Γ const &g) { for (let &x : g) *this << x; }

  virtual ~Γ() {}


  virtual Ξ operator()(Ξ &x)
    { Ξ y{x}; for (let &g : gs) y = (*g)(y); return y; }

  virtual St name() const
    { St n; for (let &x : gs) n.append(x->name()); return n; }


  Γ &operator<<(γ* x)    { return *this << Sp<γ>(x); }
  Γ &operator>>(γ* x)    { return *this >> Sp<γ>(x); }
  Γ &operator<<(Sp<γ> x) { gs.push_back(x);  return *this; }
  Γ &operator>>(Sp<γ> x) { gs.push_front(x); return *this; }

  Γ &operator<<(Γ const &xs) { for (let &x : xs) *this << x; return *this; }
  Γ &operator>>(Γ const &xs) { for (let &x : xs) *this >> x; return *this; }


  D<Sp<γ>>::iterator       begin()       { return gs.begin(); }
  D<Sp<γ>>::iterator       end()         { return gs.end(); }
  D<Sp<γ>>::const_iterator begin() const { return gs.begin(); }
  D<Sp<γ>>::const_iterator end()   const { return gs.end(); }


protected:
  D<Sp<γ>> gs;
};


O &operator<<(O&, Γ const&);


template<class T>
Γ &operator>>(T const &x, Γ &g) { return g >> x; }


inline Γ operator|(Sp<γ>    x, Sp<γ>    y) { return Γ{x} << y; }
inline Γ operator|(Γ const &x, Sp<γ>    y) { return Γ{x} << y; }
inline Γ operator|(Sp<γ>    x, Γ const &y) { return Γ{x} << y; }
inline Γ operator|(Γ const &x, Γ const &y) { return Γ{x} << y; }


}

#include "end.hh"


#endif
