#include "pi.hh"

#include "begin.hh"

namespace τ
{


static φ<πfn> φatom_;
static φ<πfn> φatomd_;

φa_<πfn> &φatoma() { if (!φatom_)  φatom_ .reset(new φa_<πfn>); return *dpc<φa_<πfn>>(φatom_); }
φd_<πfn> &φatomd() { if (!φatomd_) φatomd_.reset(new φd_<πfn>); return *dpc<φd_<πfn>>(φatomd_); }


φ<πfn> φπ()
{
  return φm<P<πfn, V<πfn>>, πfn>(
    φs(φatom(), φn(φop())),
    [](auto x)
      { πfn r = std::get<0>(x);
        for (let &y : std::get<1>(x)) r += y;
        return r; });
}


φ<πfn> φgroup()
{
  return φm<P<πfn, Op<int>>, πfn>(φs(φπ(), φo(φl("]", 0))),
                                  [](auto x) { return std::get<0>(x); });
}


φ<πfn> φatom()
{
  static bool i = false;
  if (!i)
  {
    φatoma();
    φatomd();
    i = true;
    φatoma() << φπ_literal();
    φatoma() << φatomd_;
    φatomd().def("(", φtuple());
    φatomd().def("[", φgroup());
  }
  return φN("φπ", φws(φatom_));
}


}

#include "end.hh"
