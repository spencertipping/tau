#ifndef τγ_h
#define τγ_h


#include "tau.hh"
#include "Xi.hh"

#include "begin.hh"

namespace τ
{


struct γ;

void γc_(γ*);
void γd_(γ*);
uN   γn();


// Virtual base for γ compilers
struct γ
{
  γ()          { γc_(this); }
  virtual ~γ() { γd_(this); }
  virtual Ξ &operator()(Ξ &x) = 0;
          Ξ &operator()(Ξ &&x) { return (*this)(x); }

  virtual St name() const = 0;
};


// FIXME: struct-wrap Sp<γ> with operators so we don't have Sp<γ> in
// user code


O &operator<<(O&, γ const&);


}

#include "end.hh"


#endif
