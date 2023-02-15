#include "pi.hh"
#include "share.hh"
#include "linux.hh"
#include "wasm.hh"

#include "begin.hh"

namespace τ
{


static φ<πfn> φγ_;

φd_<πfn> &φγd()
{ if (!φγ_) φγ_.reset(new φd_<πfn>);
  return *dpc<φd_<πfn>>(φγ_); }


φ<πfn> φγ()
{
  static bool i = false;
  if (!i)
  {
    φγd();
    i = true;
    φshare(φγd());
    φlinux(φγd());
    φwasm (φγd());
  }
  return φN("φγ", φws(φγ_));
}


}

#include "end.hh"
