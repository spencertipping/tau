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


φ<πfn> φπfn()
{
  return φm<πfn, πfn>(φπ(), [](πfn f) { return f.q(); });
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
    φatomd().def("λ", φπfn());
    φatomd().def(
      "a", φnull({"η[0]", [](πi &i) { i.dpush(i.y()[0]); }}),
      "b", φnull({"η[1]", [](πi &i) { i.dpush(i.y()[1]); }}),
      "c", φnull({"η[2]", [](πi &i) { i.dpush(i.y()[2]); }}),
      "d", φnull({"η[3]", [](πi &i) { i.dpush(i.y()[3]); }}),
      "e", φnull({"η[4]", [](πi &i) { i.dpush(i.y()[4]); }}),
      "f", φnull({"η[5]", [](πi &i) { i.dpush(i.y()[5]); }}),
      "g", φnull({"η[6]", [](πi &i) { i.dpush(i.y()[6]); }}),
      "h", φnull({"η[7]", [](πi &i) { i.dpush(i.y()[7]); }}),
      "i", φnull({"η[8]", [](πi &i) { i.dpush(i.y()[8]); }}),
      "j", φnull({"η[9]", [](πi &i) { i.dpush(i.y()[9]); }}));

    φatomd().def(
      "y", φnull({"η", [](πi &i) { i.dpush(i.y()); }}));
  }
  return φN("φπ", φws(φatom_));
}


}

#include "end.hh"
