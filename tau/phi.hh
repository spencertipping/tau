#ifndef τφ_h
#define τφ_h


#include "types.hh"
#include "begin.hh"

namespace τ
{


template<class T> struct φ;
template<class T> struct φr;
struct φc;


// φ parser ∷ φc → φr<T>
template<class T>
struct φ
{
  virtual ~φ() {};

  virtual St          name()          const = 0;
  virtual φr<T> operator()(φc const&) const = 0;
};


// Parse result, which is either:
//   accept ∷ (x, i) → (y, j)
//   fail   ∷ (x, i) → (e, t, j)
template<class T>
struct φr
{
  Sp<Stc> x;
  uN      i;
  uN      j;
  Op<T>   y;
  St      e;
  St      t;

  bool is_a() const { return  y.has_value(); }
  bool is_f() const { return !y.has_value(); }

  bool operator>(φr<T> const &o) const { return j > o.j; }

  φr &operator=(φr<T> const &a)
    { x = a.x; i = a.i; j = a.j;
      y = a.y; e = a.e; t = a.t; return *this; }

  template<class U>
  φr<U> cast() const
    { A(is_f(), "cannot cast φr::a");
      return *Rc<φr<U> const*>(this);}
};


// φ input context: input string and current φ trace
struct φc
{
  // Create a new root context
  φc(St x__) : x_(new St{x__}), i_(0), n_(""), p_(nullptr) {}
  φc(Sp<Stc> x__, uN i__, Stc n__, φc const *p__)
    : x_(x__), i_(i__), n_(n__), p_(p__) {}

                    φc at(uN j)          const { return φc{x_, j, n_, p_}; }
  template<class T> φc at(φ<T> const &f) const { return φc{x_, i_, f.name(), this}; }

  template<class T> φr<T> a(T  y, uN j) const { return φr<T>{x_, i_, j, y, {}, {}}; }
  template<class T> φr<T> f(St e, uN j) const { return φr<T>{x_, i_, j, {}, e, trace()}; }

  St trace() const { St r; trace_(r); return r; }

  Stc &x() const { return *x_; }
  uN   i() const { return i_; }
  uN   l() const { return x_->size() - i_; }

  ch operator[](uN i) const { return x_->at(i_ + i); }
  St sub(uN n)        const { return x_->substr(i_, n); }
  St sub(uN s, uN n)  const { return x_->substr(i_ + s, n); }

  φc &operator=(φc const &x)
    { x_ = x.x_; i_ = x.i_;
      n_ = x.n_; p_ = x.p_; return *this; }


protected:
  Sp<Stc>   x_;  // input
  uN        i_;  // input offset
  St        n_;  // current parser name
  φc const *p_;  // parent context, or nullptr

  void trace_(St &into) const
    { if (p_) p_->trace_(into), into.append(" ");
      into.append(n_); }
};


template<class T>
O &operator<<(O &s, φ<T> const &f)
{
  return s << f.name();
}


}

#include "end.hh"


#endif
