#ifndef τφ_h
#define τφ_h


#include <typeinfo>

#include "types.hh"
#include "begin.hh"

namespace τ
{


template<class T> struct φ_;
template<class T> struct φr_;
struct φc_;


// Parser structures end in _ because we don't instantiate them directly.
// Instead, we wrap them with shared pointers. This class defines the
// user-facing API for parsers.
template<class T> struct φ;


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
//   fail   ∷ (x, i) → (p, j)
template<class T>
struct φr_ final
{
  Sp<Stc>      x;
  uN           i;
  uN           j;
  Op<T>        y;
  φ_<T> const *p;

  bool is_a() const { return  y.has_value(); }
  bool is_f() const { return !y.has_value(); }

  bool operator>(φr_<T> const &o) const { return j > o.j; }
  φr_ &operator=(φr_<T> const &a)
    { x = a.x; i = a.i; j = a.j; y = a.y; p = a.p; return *this; }

  template<class U>
  φr_<U> cast(U &&y) const
    { A(is_a(), "cannot cast(y) φr::f");
      return φr_<De<U>>{x, i, j, {std::forward<U>(y)}, Rc<φ_<U> const*>(p)}; }

  template<class U>
  φr_<U> cast() const
    { A(is_f(), "cannot cast φr::a");
      return φr_<U>{x, i, j, {}, Rc<φ_<U> const*>(p)}; }
};


// φ input context: input string and current φ trace
struct φc_ final
{
  // Create a new root context
  φc_(Stc    &x__)         : x_(new St{x__}), i_(0)   {}
  φc_(Sp<Stc> x__)         : x_(x__),         i_(0)   {}
  φc_(Sp<Stc> x__, uN i__) : x_(x__),         i_(i__) {}


  φc_ at(uN j) const { return φc_{x_, j}; }

  template<class T> φr_<T> a(T            y, uN j) const { return φr_<T>{x_, i_, j, y, {}}; }
  template<class T> φr_<T> f(φ_<T> const *p, uN j) const { return φr_<T>{x_, i_, j, {}, p}; }

  Stc &x() const { return *x_; }
  uN   i() const { return i_; }
  uN   l() const { return x_->size() - i_; }
  uN   n() const { return x_->size(); }

  ch operator[](iN i) const { return x_->at(i_ + i); }
  St sub(uN n)        const { return {x_->data() + i_, n}; }
  St sub(uN s, uN n)  const { return {x_->data() + i_ + s, n}; }

  St::const_iterator it(uN i) const { return x_->begin() + i_ + i; }


protected:
  Sp<Stc> x_;  // input
  uN      i_;  // input offset
};


template<class T>
struct φ final
{
  φ(φ_<T> *p_) : p(p_) {}

  φr_<T> operator()(φc_ const &x) const { return (*p)(x); }
  φr_<T> operator()(Stc       &x) const { return (*p)(φc_(x)); }

  St name() const { return p->name(); }

  template<class X>
  X &as() const { return *dynamic_cast<X*>(p.get()); }

  Op<T> parse(Stc &x) const { return (*this)(x).y; }

  φ_<T> &operator*() const { return *p; }

  Sp<φ_<T>> p;
};


template<class T> O &operator<<(O &s, φ_<T> const &f) { return s << f.name(); }
template<class T> O &operator<<(O &s, φ <T> const &f) { return s << f.name(); }


}

#include "end.hh"


#endif
