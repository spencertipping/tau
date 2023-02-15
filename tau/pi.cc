#include "pi.hh"
#include "share.hh"
#include "linux.hh"
#include "wasm.hh"

#include "begin.hh"

namespace τ
{


φ<πfn> φγ()
{
  static bool i = false;
  static let  r = new φd_<πfn>;
  if (!i)
  {
    φshare(*r);
    φlinux(*r);
    φwasm(*r);
  }
  return Sp<φ_<πfn>>(r);
}


}

#include "end.hh"
