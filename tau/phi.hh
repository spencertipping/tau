#ifndef τφ_h
#define τφ_h


#include "types.hh"
#include "begin.hh"

namespace τ
{


template<class T> struct φ_;
template<class T> struct φr_;
struct φc_;


// Parser structures end in _ because we don't instantiate them directly.
// Instead, we wrap them with shared pointers and build them with ctors.
template<class T> using φ = Sp<φ_<T>>;


// φ parser ∷ φc → φr<T>
template<class T>
struct φ_
{
  virtual ~φ_() {};

  virtual St           name()           const = 0;
  virtual φr_<T> operator()(φc_ const&) const = 0;
};


// Parse result, which is either:
//   accept ∷ (x, i) → (y, j)
//   fail   ∷ (x, i) → (e, t, j)
template<class T>
struct φr_ final
{
  Sp<Stc> x;
  uN      i;
  uN      j;
  Op<T>   y;
  St      e;
  St      t;

  bool is_a() const { return  y.has_value(); }
  bool is_f() const { return !y.has_value(); }

  bool operator>(φr_<T> const &o) const { return j > o.j; }

  φr_ &operator=(φr_<T> const &a)
    { x = a.x; i = a.i; j = a.j;
      y = a.y; e = a.e; t = a.t; return *this; }

  template<class U>
  φr_<U> cast() const
    { A(is_f(), "cannot cast φr::a");
      return φr_<U>{x, i, j, {}, e, t}; }
};


// φ input context: input string and current φ trace
struct φc_ final
{
  // Create a new root context
  φc_(St x__) : x_(new St{x__}), i_(0), n_(""), p_(nullptr) {}
  φc_(Sp<Stc> x__, uN i__, Stc n__, φc_ const *p__)
    : x_(x__), i_(i__), n_(n__), p_(p__) {}

                    φc_ at(uN j)           const { return φc_{x_, j, n_, p_}; }
  template<class T> φc_ at(φ_<T> const &f) const { return φc_{x_, i_, f.name(), this}; }

  template<class T> φr_<T> a(T  y, uN j) const { return φr_<T>{x_, i_, j, y, {}, {}}; }
  template<class T> φr_<T> f(St e, uN j) const { return φr_<T>{x_, i_, j, {}, e, trace()}; }

  St trace() const { St r; trace_(r); return r; }

  Stc &x() const { return *x_; }
  uN   i() const { return i_; }
  uN   l() const { return x_->size() - i_; }
  uN   n() const { return x_->size(); }

  ch operator[](iN i) const { return x_->at(i_ + i); }
  St sub(uN n)        const { return {x_->data() + i_, n}; }
  St sub(uN s, uN n)  const { return {x_->data() + i_ + s, n}; }

  St::const_iterator it(uN i) const { return x_->begin() + i_ + i; }

  φc_ &operator=(φc_ const &x)
    { x_ = x.x_; i_ = x.i_;
      n_ = x.n_; p_ = x.p_; return *this; }


protected:
  Sp<Stc>    x_;  // input
  uN         i_;  // input offset
  St         n_;  // current parser name
  φc_ const *p_;  // parent context, or nullptr

  void trace_(St &into) const
    { if (p_) p_->trace_(into), into.append(" ");
      into.append(n_); }
};


template<class T>
O &operator<<(O &s, φ_<T> const &f)
{
  return s << f.name();
}


}

#include "end.hh"


#endif
