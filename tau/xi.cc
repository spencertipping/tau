#include "xi.hh"
#include "begin.hh"

namespace τ
{


static iN ξs_ = 0;

void ξc_(ξ *c) { ++ξs_; }
void ξd_(ξ *c) { A(--ξs_ >= 0, "ξs underflow"); }
uN   ξn()      { return ξs_; }


O &operator<<(O &s, ξ const &y)
{
  return s << "ξ[" << (y.wc ? "#" : "")
           << "wt=" << y.wt() << " " << y.z << "]";
}


}

#include "end.hh"
