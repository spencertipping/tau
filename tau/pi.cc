#include "pi.hh"
#include "share.hh"
#include "linux.hh"
#include "wasm.hh"

#include "begin.hh"

namespace τ
{


slet φγ_  = Sp<φ_<πfn>>(new φd_<πfn>);
slet φπ_  = Sp<φ_<πfn>>(new φa_<πfn>);
slet φπd_ = Sp<φ_<πfn>>(new φd_<πfn>);


φd_<πfn> &φγd() { return *dpc<φd_<πfn>>(φγ_); }
φa_<πfn> &φπa() { return *dpc<φa_<πfn>>(φπ_); }
φd_<πfn> &φπd() { return *dpc<φd_<πfn>>(φπd_); }


φ<πfn> φγ()
{
  static bool i = false;
  if (!i)
  {
    φshare(φγd());
    φlinux(φγd());
    φwasm (φγd());
  }
  return φγ_;
}


φ<πfn> φπ()
{
  static bool i = false;
  if (!i)
  {
    φπa() << φπ_literal();
    φπa() << φπd_;
  }
  return φπ_;
}


}

#include "end.hh"
