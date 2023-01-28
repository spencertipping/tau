#ifndef τψ_h
#define τψ_h


#include "lambda-types.hh"
#include "Lambda.hh"
#include "begin.hh"

namespace τ
{


// Virtual base for ψ processes
struct ψ
{
  ψ(Λ &l_) : l(l_) {}
  virtual ~ψ() { for (let x : ls) l.x(x); }

  Stc &name() const {            return name_; }
  ψ   &name(Stc &s) { name_ = s; return *this; }


protected:
  Λ    &l;
  S<λi> ls;
  St    name_;  // name for debugging purposes
};


}

#include "end.hh"


#endif
