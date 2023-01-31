#ifndef τφfn_h
#define τφfn_h


#include "types.hh"
#include "phi.hh"
#include "begin.hh"

namespace τ
{


// Preferential alternative
template<class T>
struct φa : public virtual φ<T>
{
  φa() {}
  φa(Il<Sp<φ<T>>> ps_) : ps(ps_) {}

  St name() const;
  φr<T> operator()(φc const &x) const;

  φa<T> &operator<<(Sp<φ<T>> p) { ps.push_back(p);  return *this; }
  φa<T> &operator>>(Sp<φ<T>> p) { ps.push_front(p); return *this; }

  D<Sp<φ<T>>> ps;
};


// Repetition
template<class T>
struct φn : public virtual φ<V<T>>
{
  φn(Sp<φ<T>> p_, uN min_ = 0, uN max_ = -1)
    : p(p_), min(min_), max(max_) {}

  St name() const;
  φr<V<T>> operator()(φc const &x) const;

  Sp<φ<T>> p;
  uN       min;
  uN       max;
};


// Sequential parsing
template<class... Xs>
struct φs : public virtual φ<T<Xs...>>
{
  template<class T> struct sp { using type = Sp<φ<T>>; };

  φs(typename sp<Xs>::type... ps_) : ps(ps_...) {}

  St name() const { return "(" + name_<sizeof...(Xs) - 1>() + ")"; }

  // TODO: figure out template logic for this one
  //φr<T<Xs...>> operator()(φc const &x) const { return parse_<>(x); }

  T<typename sp<Xs>::type...> ps;


protected:
  template<uN i>
  typename std::enable_if<1 <= i, St>::type
  name_() const { return name_<i - 1>() + ", " + std::get<i>(ps)->name(); }

  template<uN i>
  typename std::enable_if<i == 0, St>::type
  name_() const { return std::get<0>(ps)->name(); }
};


}

#include "end.hh"


#endif
