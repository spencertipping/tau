#include "gamma.hh"
#include "begin.hh"

namespace τ
{


static iN γs_ = 0;

void γc_(γ_ *c) { ++γs_; }
void γd_(γ_ *c) { --γs_; }
uN   γn()       { A(γs_ >= 0, "γs underflow"); return γs_; }


O &operator<<(O &s, γ const &g)
{
  return s << g.name();
}


}

#include "end.hh"
