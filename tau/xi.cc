#include "xi.hh"
#include "begin.hh"

namespace τ
{


static iN ξs_ = 0;

void ξc_(ξ *c) { ++ξs_; }
void ξd_(ξ *c) { --ξs_; }
uN   ξn()      { A(ξs_ >= 0, "ξs underflow"); return ξs_; }


O &operator<<(O &s, ξ const &y)
{
  return s << "ξ[" << (y.wc ? "#" : "")
           << "wt=" << y.wt() << " " << y.z << "]";
}


}

#include "end.hh"
