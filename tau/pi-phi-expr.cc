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
    φatomd().def("{", φmap());
    φatomd().def("[", φgroup());
    φatomd().def(
      "a", φl("", πfn{πinsn{"t[0]", [](πi &i) { i.dpush(i.y()[0]); return πinsn_ok; }}}),
      "b", φl("", πfn{πinsn{"t[1]", [](πi &i) { i.dpush(i.y()[1]); return πinsn_ok; }}}),
      "c", φl("", πfn{πinsn{"t[2]", [](πi &i) { i.dpush(i.y()[2]); return πinsn_ok; }}}),
      "d", φl("", πfn{πinsn{"t[3]", [](πi &i) { i.dpush(i.y()[3]); return πinsn_ok; }}}),
      "e", φl("", πfn{πinsn{"t[4]", [](πi &i) { i.dpush(i.y()[4]); return πinsn_ok; }}}),
      "f", φl("", πfn{πinsn{"t[5]", [](πi &i) { i.dpush(i.y()[5]); return πinsn_ok; }}}),
      "g", φl("", πfn{πinsn{"t[6]", [](πi &i) { i.dpush(i.y()[6]); return πinsn_ok; }}}),
      "h", φl("", πfn{πinsn{"t[7]", [](πi &i) { i.dpush(i.y()[7]); return πinsn_ok; }}}),
      "i", φl("", πfn{πinsn{"t[8]", [](πi &i) { i.dpush(i.y()[8]); return πinsn_ok; }}}),
      "j", φl("", πfn{πinsn{"t[9]", [](πi &i) { i.dpush(i.y()[9]); return πinsn_ok; }}}));
  }
  return φN("φπ", φws(φatom_));
}


}

#include "end.hh"
