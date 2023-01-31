#include <algorithm>

#include "phi-str.hh"
#include "phi-inst.hh"
#include "begin.hh"

namespace τ
{


#define F(T) \
  template struct φd_<T>; \
  template struct φl_<T>;

φinst(F)


template<class T>
St φd_<T>::name() const
{
  St r = "{\n";
  for (let &[k, p] : ps) r += "\"" + k + "\" → " + p->name() + "\n";
  return r + "}";
}

template<class T>
φr_<T> φd_<T>::operator()(φc_ const &x) const
{
  V<St> ks;
  for (let &[k, p] : ps)
    if (k.size() <= x.l() && x.sub(k.size()) == k)
      ks.push_back(k);

  std::sort(ks.begin(), ks.end(),
            [](Stc &a, Stc &b) { return a.size() > b.size(); });
  for (let &k : ks)
  {
    let s = (*ps.at(k))(x);
    if (s.is_a()) return s;
  }

  return x.at(*this).template f<T>("no matching prefix", x.i());
}


template<class T>
φr_<T> φl_<T>::operator()(φc_ const &x) const
{
  return x.l() >= l.size() && x.sub(l.size()) == l
       ? x.a(y, x.i() + l.size())
       : x.f<T>("literal mismatch", x.i());
}


φr_<St> φcs_::operator()(φc_ const &x) const
{
  St r;
  uN i = x.i();
  while (i < x.l() && cs[x[i]]) r += x[i];
  return x.a(r, x.i() + r.size());
}


}

#include "end.hh"
