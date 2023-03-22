#include "../tau.hh"

#include "share.hh"
#include "linux.hh"
#include "wasm.hh"

#include "begin.hh"

namespace τ
{


static φ<πfn> φΓ_(nullptr);

φd_<πfn> &φΓd()
{
  if (!φΓ_.p) φΓ_.p.reset(new φd_<πfn>);
  return *dpc<φd_<πfn>>(φΓ_.p);
}


φ<πfn> φΓ()
{
  return φN("φΓ", φm<V<πfn>, πfn>(φn(φws(φΓ1()), 1), [](V<πfn> fs)
    { πfn r;
      bool first = true;
      for (let &x : fs)
      { r += x;
        if (first) first = false;
        else       r << πinsn{"Γ|", [](πi &i)
          { let b = i.dpop().as_Γ();
            let a = i.dpop().as_Γ();
            i.dpush(a | b); }}; }
      return r; }));
}


φ<πfn> φΓ1()
{
  static bool i = false;
  if (!i)
  {
    φΓd();
    i = true;

    φΓd().def("[", φm<P<πfn, Op<int>>, πfn>(
      φs(φΓ(), φo(φl("]", 0))),
      [](auto x) { return std::get<0>(x); }));

    φshare(φΓd());
    φlinux(φΓd());
    φwasm (φΓd());
  }
  return φN("φΓ₁", φΓ_);
}


}

#include "end.hh"
