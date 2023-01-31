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


// Functional transform (map)
template<class T, class U>
struct φm : public virtual φ<U>
{
  φm(Sp<φ<T>> p_, F<U(T)> f_) : p(p_), f(f_) {}

  St name() const { return "f(" + p->name() + ")"; }
  φr<U> operator()(φc const &x) const
    { let s = (*p)(x);
      return s.is_f()
           ? s.template cast<U>()
           : φr<U>{s.x, s.i, s.j, f(*s.y), s.e, s.t}; }

  Sp<φ<T>> p;
  F<U(T)>  f;
};


// Filter
template<class T>
struct φf : public virtual φ<T>
{
  φf(Sp<φ<T>> p_, F<bool(T)> f_) : p(p_), f(f_) {}

  St name() const { return "(" + p->name() + " | f)"; }
  φr<T> operator()(φc const &x) const
    { let s = (*p)(x);
      return !s.is_a() || f(*s.y)
           ? s
           : x.at(*this).f("predicate rejected", s.j); }

  Sp<φ<T>>   p;
  F<bool(T)> f;
};


}

#include "end.hh"


#endif
