#include "phi-fn.hh"
#include "phi-inst.hh"
#include "begin.hh"

namespace τ
{


#define F(T) \
  template struct φa<T>; \
  template struct φn<T>;

φinst(F)


// φa<T>
//
template<class T>
St φa<T>::name() const
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
φr<T> φa<T>::operator()(φc const &x) const
{
  φr<T> r;
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
St φn<T>::name() const
{
  return p->name() + (Ss{} << "{" << min << "," << max << "}").str();
}

template<class T>
φr<V<T>> φn<T>::operator()(φc const &x) const
{
  V<φr<T>> r;
  φc y = x;
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


}

#include "end.hh"
