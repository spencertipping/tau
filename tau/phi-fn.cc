#include "phi-fn.hh"
#include "phi-inst.hh"
#include "begin.hh"

namespace τ
{


#define F(T) \
  template struct φa_<T>; \
  template struct φn_<T>; \
  template struct φo_<T>;

φinst(F)


// φa<T>
//
template<class T>
St φa_<T>::name() const
{
  St   r     = "(";
  bool first = true;
  for (let &p : ps)
  {
    if (first) first = false;
    else       r.append(" | ");
    r.append(p->name());
  }
  return r.append(")");
}

template<class T>
φr_<T> φa_<T>::operator()(φc_ const &x) const
{
  φr_<T> r;
  bool  first = true;
  for (let &p : ps)
  {
    let s = (*p)(x);
    if (first || s > r) r = s;
    if (r.is_a()) return r;
  }
  return r;
}


// φn<T>
//
template<class T>
St φn_<T>::name() const
{
  return p->name() + (Ss{} << "{" << min << "," << max << "}").str();
}

template<class T>
φr_<V<T>> φn_<T>::operator()(φc_ const &x) const
{
  V<φr_<T>> r;
  φc_ y = x;
  for (uN i = 0; i < max; ++i)
  {
    let s = (*p)(y);
    if (s.is_f()) return s.template cast<V<T>>();
    r.push_back(s);
    y = y.at(s.j);
  }
  if (r.size() < min)
    return x.at(*this).template f<V<T>>("too few elements", y.i());

  V<T> rs;
  for (let &z : r) rs.push_back(*z.y);
  return x.a(rs, r.empty() ? x.i() : r.back().i);
}


// φo<T>
//
template<class T>
φr_<Op<T>> φo_<T>::operator()(φc_ const &x) const
{
  let s = (*p)(x);
  return x.template a<Op<T>>(s.y, s.is_a() ? s.j : x.i());
}


}

#include "end.hh"
