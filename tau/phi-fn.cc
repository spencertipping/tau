#include "phi-fn.hh"
#include "begin.hh"

namespace τ
{


// φa<T>
//
template<class T>
St φa<T>::name() const
{
  St   r     = "(";
  bool first = true;
  for (let p : ps)
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
  for (let p : ps)
  {
    let s = p(x);
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
  return p.name() + "{" + min + "," + max + "}";
}

template<class T>
φr<V<T>> φn<T>::operator()(φc const &x) const
{
  V<φr<T>> r;
  φc y = x;
  for (uN i = 0; i < max; ++i)
  {
    let s = p(y);
    if (s.is_f()) return s;
    r.push_back(s);
    y = y.at(s.j);
  }
  if (r.size() < min)
    return x.at(*this).f("too few elements", y.i);
  return r;
}


}

#include "end.hh"
