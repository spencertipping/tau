#include "pi.hh"
#include "share.hh"
#include "linux.hh"
#include "wasm.hh"

#include "begin.hh"

namespace τ
{


φ<πef> φγ()
{
  static bool i = false;
  static let  r = new φd_<πef>;
  if (!i)
  {
    φshare(*r);
    φlinux(*r);
    φwasm(*r);
  }
  return Sp<φ_<πef>>(r);
}


}

#include "end.hh"
