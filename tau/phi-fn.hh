#ifndef τφfn_h
#define τφfn_h


#include "types.hh"
#include "phi.hh"
#include "begin.hh"

namespace τ
{


// Constant name
template<class T>
struct φnamed_ : public virtual φ_<T>
{
  φnamed_(St name__, φ<T> p_) : name_(name__), p(p_) {}

  St           name()             const { return name_; }
  φr_<T> operator()(φc_ const &x) const { return (*p)(x); }

  St   name_;
  φ<T> p;
};


// Preferential alternative
template<class T>
struct φa_ : public virtual φ_<T>
{
  template<class... Xs>
  φa_(Xs const&... xs) { push(xs...); }

  St name() const
    { St   r     = "(";
      bool first = true;
      for (let &p : ps)
      { if (first) first = false;
        else       r.append(" | ");
        r.append(p->name()); }
      return r.append(")"); }

  φr_<T> operator()(φc_ const &x) const
    { φr_<T> r;
      bool  first = true;
      for (let &p : ps)
      { let s = (*p)(x);
        if (first || s > r) r = s;
        if (r.is_a()) return r; }
      return r; }

  φa_<T> &operator<<(φ<T> p) { A(p, "φa<<∅"); ps.push_back(p);  return *this; }
  φa_<T> &operator>>(φ<T> p) { A(p, "φa>>∅"); ps.push_front(p); return *this; }

  template<class... Xs>
  φa_<T> &push(φ<T> p, Xs const&... xs) { *this << p; return push(xs...); }
  φa_<T> &push()                        {             return *this; }

  D<φ<T>> ps;
};


// Repetition
template<class T>
struct φn_ : public virtual φ_<V<T>>
{
  φn_(φ<T> p_, uN min_ = 0, uN max_ = -1)
    : p(p_), min(min_), max(max_) {}

  St name() const
    { return p->name() + (Ss{} << "{" << min << "," << max << "}").str(); }

  φr_<V<T>> operator()(φc_ const &x) const
    { V<φr_<T>> r;
      φc_ y = x;
      for (uN i = 0; i < max; ++i)
      { let s = (*p)(y);
        if (s.is_f()) break;
        r.push_back(s);
        y = y.at(s.j); }
      if (r.size() < min)
        return x.at(*this).template f<V<T>>("too few elements", y.i());
      V<T> rs;
      for (let &z : r) rs.push_back(*z.y);
      return x.a(rs, r.empty() ? x.i() : r.back().j); }

  φ<T> p;
  uN   min;
  uN   max;
};


// Sequence of same-typed thing
template<class T>
struct φS_ : public virtual φ_<V<T>>
{
  template<class... Xs>
  φS_(Xs const&... ps_) : ps({ps_...}) {}

  St name() const
    { Ss r;
      r << "φs[";
      for (let &p : ps) r << p->name() << " ";
      r << "]";
      return r.str(); }

  φr_<V<T>> operator()(φc_ const &x) const
    { V<T> r;
      φc_ y = x.at(*this);
      for (let &p : ps)
      { let s = (*p)(y);
        if (s.is_f()) return s.template cast<V<T>>();
        r.push_back(*s.y);
        y = y.at(s.j); }
      return y.a(r, y.i()); }

  V<φ<T>> ps;
};


// Optional
template<class T>
struct φo_ : public virtual φ_<Op<T>>
{
  φo_(φ<T> p_) : p(p_) {}

  St name() const { return p->name() + "?"; }
  φr_<Op<T>> operator()(φc_ const &x) const
    { let s = (*p)(x);
      return x.template a<Op<T>>(s.y, s.is_a() ? s.j : x.i()); }

  φ<T> p;
};


// Optional with default
template<class T>
struct φO_ : public virtual φ_<T>
{
  φO_(φ<T> p_, T d_) : p(p_), d(d_) {}

  St name() const { return p->name() + "?_"; }
  φr_<T> operator()(φc_ const &x) const
    { let s = (*p)(x);
      return x.a(
        s.is_a() ? *s.y : d,
        s.is_a() ?  s.j : x.i()); }

  φ<T> p;
  T    d;
};


// Sequential parsing
// TODO: fully generic template for this so we can use tuples
template<class T, class U>
struct φs_ : public virtual φ_<P<T, U>>
{
  φs_(φ<T> pt_, φ<U> pu_) : pt(pt_), pu(pu_) {}

  St name() const { return pt->name() + " » " + pu->name(); }
  φr_<P<T, U>> operator()(φc_ const &x) const
    { let s = (*pt)(x);                   if (s.is_f()) return s.template cast<P<T, U>>();
      let t = (*pu)(x.at(*this).at(s.j)); if (t.is_f()) return t.template cast<P<T, U>>();
      return x.a(mp(*s.y, *t.y), t.j); }

  φ<T> pt;
  φ<U> pu;
};

template<class... Xs>
struct φs2_ : public virtual φ_<T<Xs...>>
{
  φs2_(φ<Xs>... p_) : p(p_...) {}

  template<uN i>
  If<i < sizeof...(Xs), St> name() const
    { return std::get<i>(p).name() + " " + name<i + 1>(); }

  template<uN i>
  If<i == sizeof...(Xs), St> name() const { return ""; }

  φr_<T<Xs...>> operator()(φc_ const &x) const
    { T<φr_<Xs...>> rs;
      sletc n = std::tuple_size_v<decltype(rs)>;
      iN f = (std::get<0>(rs) = std::get<0>(p)(x)).is_f() ? 0 : -1;

    }

  T<φ<Xs...>> p;
};

/*
Courtesy of chatGPT:

template<typename... Ts>
std::variant<fail_type, std::tuple<Ts...>> f(int v)
{
  std::tuple<std::optional<Ts>...> results;
  std::get<0>(results) = Ts::get(v);
  bool any_empty = !std::get<0>(results);
  int empty_index = -1;
  ((empty_index == -1 && !std::get<std::tuple_size<decltype(results)>::value - sizeof...(Ts)>(results)
    ? empty_index = std::tuple_size<decltype(results)>::value - sizeof...(Ts)
    : empty_index,
   any_empty |= !(std::get<std::tuple_size<decltype(results)>::value - sizeof...(Ts)>(results)
                  = Ts::get(*std::get<std::tuple_size<decltype(results)>::value - sizeof...(Ts) - 1>(results))),
   any_empty) || ...);
  if (any_empty) {
    return fail_type{ empty_index };
  }
  return std::make_tuple((*std::get<Ts>(results)).v...);
}
 */


// Functional transform (map)
template<class T, class U>
struct φm_ : public virtual φ_<U>
{
  φm_(φ<T> p_, F<U(T)> f_) : p(p_), f(f_) {}

  St name() const { return "f(" + p->name() + ")"; }
  φr_<U> operator()(φc_ const &x) const
    { let s = (*p)(x);
      return s.is_f()
           ? s.template cast<U>()
           : φr_<U>{s.x, s.i, s.j, f(*s.y), s.e, s.t}; }

  φ<T>    p;
  F<U(T)> f;
};


// Filter
template<class T>
struct φf_ : public virtual φ_<T>
{
  φf_(φ<T> p_, F<bool(T)> f_) : p(p_), f(f_) {}

  St name() const { return "(" + p->name() + " | f)"; }
  φr_<T> operator()(φc_ const &x) const
    { let s = (*p)(x);
      return !s.is_a() || f(*s.y)
           ? s
           : x.at(*this).f("predicate rejected", s.j); }

  φ<T>       p;
  F<bool(T)> f;
};


}

#include "end.hh"


#endif
