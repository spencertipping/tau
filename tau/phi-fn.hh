#ifndef τφfn_h
#define τφfn_h


#include "types.hh"
#include "phi.hh"
#include "begin.hh"

namespace τ
{


// Lazy parser, initialized on-demand
Tt struct φL_ : public virtual φ_<T>
{
  φL_(F<φ<T>()> f_) : φ_<T>("lazy"), f(f_), c(false) {}

  φr_<T> operator()(φc_ const &x) const noexcept
    { A(!c, "circular dependency in lazy parser");
      if (!p) c = true, p = f(), φ_<T>::name_ = p.name(), f = nullptr, c = false;
      return p(x); }

  φ<T>      mutable p;
  F<φ<T>()> mutable f;
  bool      mutable c;
};


// Weakly referenced parser
Tt struct φW_ : public virtual φ_<T>
{
  φW_(φ<T> p_) : φ_<T>(p_.name()), p(p_.p) {}
  φr_<T> operator()(φc_ const &x) const noexcept { return (*wpg(p))(x); }
  Wp<φ_<T>> p;
};


// Insta-fail parser
Tt struct φF_ : public virtual φ_<T>
{
  φF_() : φ_<T>("fail") {}
  φr_<T> operator()(φc_ const &x) const noexcept { return x.f(this, x.i()); }
};


// Preferential alternative
Tt struct φa_ : public virtual φ_<T>
{
  template<class... Xs>
  φa_(St name, Xs const&... xs) : φ_<T>(name) { push(xs...); }

  φr_<T> operator()(φc_ const &x) const noexcept
    { φr_<T> r;
      bool   first = true;
      for (let &p : ps)
      { let s = p(x);
        if (first || s > r) r = s;
        if (r.is_a()) return r; }
      return r; }

  φa_<T> &operator<<(φ<T> p) { ps.push_back(p);  return *this; }
  φa_<T> &operator>>(φ<T> p) { ps.push_front(p); return *this; }

  template<class... Xs>
  φa_<T> &push(φ<T> p, Xs const&... xs) { *this << p; return push(xs...); }
  φa_<T> &push()                        {             return *this; }

  D<φ<T>> ps;
};


// Return without consuming anything
Tt struct φR_ : public virtual φ_<T>
{
  φR_(T x__) : φ_<T>("φR"), x_(x__) {}
  φr_<T> operator()(φc_ const &x) const noexcept { return x.a(x_, x.i()); }
  T x_;
};


// Repetition
Tt struct φn_ : public virtual φ_<V<T>>
{
  φn_(φ<T> p_, uN min_ = 0, uN max_ = -1)
    : φ_<V<T>>("(" + p_.name() + ")*"), p(p_), min(min_), max(max_) {}

  φr_<V<T>> operator()(φc_ const &x) const noexcept
    { V<φr_<T>> r;
      φc_ y = x;
      for (uN i = 0; i < max; ++i)
      { let s = p(y);
        if (s.is_f()) break;
        r.push_back(s);
        y = y.at(s.j); }
      if (r.size() < min)
        return x.template f<V<T>>(this, y.i());
      V<T> rs;
      for (let &z : r) rs.push_back(z.r());
      return x.a(rs, r.empty() ? x.i() : r.back().j); }

  φ<T> p;
  uN   min;
  uN   max;
};


// Sequence of same-typed thing
Tt struct φS_ : public virtual φ_<V<T>>
{
  template<class... Xs>
  φS_(St name, Xs const&... ps_) : φ_<V<T>>(name), ps({ps_...}) {}

  φr_<V<T>> operator()(φc_ const &x) const noexcept
    { V<T> r;
      φc_ y = x;
      for (let &p : ps)
      { let s = p(y);
        if (s.is_f()) return s.template cast<V<T>>();
        r.push_back(s.r());
        y = y.at(s.j); }
      return y.a(r, y.i()); }

  V<φ<T>> ps;
};


// Optional
Tt struct φo_ : public virtual φ_<Op<T>>
{
  φo_(φ<T> p_) : φ_<Op<T>>("(" + p_.name() + ")?"), p(p_) {}

  φr_<Op<T>> operator()(φc_ const &x) const noexcept
    { let s = p(x);
      return s.is_a()
           ? x.template a<Op<T>>({s.r()}, s.j)
           : x.template a<Op<T>>({},      x.i()); }

  φ<T> p;
};


// Optional with default
Tt struct φO_ : public virtual φ_<T>
{
  φO_(φ<T> p_, T d_) : φ_<T>("(" + p_.name + ")?_"), p(p_), d(d_) {}

  φr_<T> operator()(φc_ const &x) const noexcept
    { let s = p(x);
      return x.a(s.is_a() ? s.r() : d,
                 s.is_a() ? s.j   : x.i()); }

  φ<T> p;
  T    d;
};


// Sequential parsing
template<class... Xs>
struct φs_ : public virtual φ_<T<Xs...>>
{
  φs_(St name, φ<Xs>... p_) : φ_<T<Xs...>>(name), p(p_...) {}

  φr_<T<Xs...>> operator()(φc_ const &x) const noexcept
    { return res<0>(go<0>(x, x.a(true, x.i()))); }

  T<φ<Xs>...> p;


protected:
  template<uS i, class X>
  Tdrop<i, T<φr_<Xs>...>> go(φc_ const &x, φr_<X> const &r0) const noexcept
    { if constexpr (i == sizeof...(Xs)) return {};
      else
      { using Y = De<decltype(std::get<i>(std::declval<T<Xs...>>()))>;
        let r = r0.is_f() ? r0.template cast<Y>() : std::get<i>(p)(x);
        return tcons(r, go<i + 1>(x.at(r.j), r)); } }

  template<uS i>
  φr_<Tdrop<i, T<Xs...>>> res(T<φr_<Xs>...> const &xs) const noexcept
    { let x = std::get<i>(xs); if (x.is_f()) return x.template cast<Tdrop<i, T<Xs...>>>();
      if constexpr (i + 1 >= sizeof...(Xs)) return x.cast(std::make_tuple(x.r()));
      else
      { let y = res<i + 1>(xs); if (y.is_f()) return y.template cast<Tdrop<i, T<Xs...>>>();
        return y.cast(tcons(x.r(), y.r())); } }
};


// Functional transform (map)
template<class T,
         class F,
         class U = decltype(std::declval<F>()(std::declval<T>()))>
struct φm_ : public virtual φ_<U>
{
  φm_(φ<T> p_, F f_)
    : φ_<U>("f(" + p_.name() + ")"), p(p_), f(f_) {}

  φr_<U> operator()(φc_ const &x) const noexcept
    { let s = p(x);
      return s.is_f()
           ? s.template cast<U>()
           : s.template cast<U>(f(s.r())); }

  φ<T> p;
  F    f;
};


// Flatmap φr_<T> to φr_<U>
template<class T,
         class F,
         class U = typename decltype(
           std::declval<F>()(std::declval<φc_>(),
                             std::declval<φr_<T>>()))::R>
struct φM_ : public virtual φ_<U>
{
  φM_(φ<T> p_, F f_)
    : φ_<U>("f(" + p_.name() + ")"), p(p_), f(f_) {}

  φr_<U> operator()(φc_ const &x) const noexcept
    { auto s = p(x);
      return s.is_f() ? s.template cast<U>() : f(x, mo(s)); }

  φ<T> p;
  F    f;
};


// Filter
Tt struct φf_ : public virtual φ_<T>
{
  φf_(φ<T> p_, F<bool(T)> f_)
    : φ_<T>("(" + p_.name() + " | f)"), p(p_), f(f_) {}

  φr_<T> operator()(φc_ const &x) const noexcept
    { let s = p(x);
      return !s.is_a() || f(s.r()) ? s : x.f(this, s.j); }

  φ<T>       p;
  F<bool(T)> f;
};


}

#include "end.hh"


#endif
