#ifndef τγ_h
#define τγ_h


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

  virtual St name() const = 0;
};


O &operator<<(O&, γ const&);


}

#include "end.hh"


#endif
