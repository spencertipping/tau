#ifndef tau_flux_ζ_h
#define tau_flux_ζ_h


#include "../types.hh"

#include "Lambda.hh"
#include "zetab.hh"
#include "zetabits.hh"


#include "../module/begin.hh"

namespace tau::flux
{


// TODO: define serialization protocol used by T

template<class T>
struct ζr;

template<class T>
struct ζ
{
  Λ                 &l;
  P<uf8c, ζi> const  i;
  ζb                 b;

  ζ(Λ &l_, uf8 b_ = ζb0) : l(l_), i(ζni(b_, this)), b(b_) {}
  ~ζ() { ζs[std::get<0>(i)][std::get<1>(i)] = nullptr; }
};


template<class T>  // FIXME: need RC here too
struct ζr
{
  uNc i;
  ζr(uN i_) : i(i_) {}

  uf8 bi() const { return ζbi(i); }
  ζi  zi() const { return ζzi(i); }
  uN  xi() const { return ζxi(i); }

  ζ<T> &z() const { return *Rc<ζ<T>*>(ζs[zi()]); }

  operator T() const { let &z_ = z(); return T(z_, z_.start(xi())); }
};


#if tau_debug_iostream
template<class T>
O &operator<<(O &s, ζr<T> const &x)
{
  return s << "ζr" << x.bi() << ":" << x.zi() << ":" << x.xi();
}


template<class T>
O &operator<<(O &s, ζ<T> const &x)
{
  return s << "ζ" << std::get<0>(x.i) << ":" << std::get<1>(x.i)
           << x.b;
}
#endif


}

#include "../module/end.hh"


#endif
