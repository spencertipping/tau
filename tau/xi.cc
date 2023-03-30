#include "xi.hh"
#include "begin.hh"

namespace τ
{


#if τtrack_ξ
  static S<ξ*> ξs_;

  void ξc_(ξ *c) { ξs_.insert(c); }
  void ξd_(ξ *c) { ξs_.erase(c); }
  uN   ξn() { return ξs_.size(); }

  S<ξ*> &ξs() { return ξs_; }
#else
  static iN ξs_ = 0;

  void ξc_(ξ *c) { ++ξs_; }
  void ξd_(ξ *c) { A(--ξs_ >= 0, "ξs underflow"); }
  uN   ξn()      { return ξs_; }
#endif


O &operator<<(O &s, ξ const &y)
{
  return s << "ξ[" << (y.wc ? "#" : "")
           << "wt=" << y.wt << " " << y.z << "]";
}


}

#include "end.hh"
