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
  φ_(St name) : name_(name) {}
  virtual ~φ_() {};

  virtual St           name()           const noexcept { return name_; }
  virtual φr_<T> operator()(φc_ const&) const noexcept = 0;

  St name_;
};


// Parse result, which is either:
//   accept ∷ (x, i) → (y, j)
//   fail   ∷ (x, i) → (p, j)
template<class T>
struct φr_ final
{
  uN                  i;
  uN                  j;
  Va<φ_<T> const*, T> y;

  bool is_a() const { return y.index() == 1; }
  bool is_f() const { return y.index() == 0; }

  φ_<T> const &p() const { return *std::get<0>(y); }
  T     const &r() const
    { A(is_a(), "parse failure at index " << j << ", parser " << p().name());
      return  std::get<1>(y); }

  bool operator>(φr_<T> const &o) const noexcept { return j > o.j; }
  φr_ &operator=(φr_<T> const &a) noexcept
    { i = a.i; j = a.j; y = a.y; return *this; }

  template<class U>
  φr_<U> cast(U &&y) const noexcept
    { A(is_a(), "cannot cast(y) φr::f");
      return φr_<De<U>>{i, j, {std::forward<U>(y)}}; }

  template<class U>
  φr_<U> cast() const noexcept
    { A(is_f(), "cannot cast() φr::a");
      return φr_<U>{i, j, Rc<φ_<U> const*>(&p())}; }
};


typedef u64 φid_;  // parser ID
typedef u64 φmk_;  // memo key

φid_ φgen_id();

ic φmk_ φmk(φid_ p, uN i) { return p << 32 | i; }


// φ input context: input string, current position, and shared memo cache
struct φc_ final
{
  φc_(Stc &x__)
    : x_(new St{x__}),
      m_(new M<φmk_, φ_<void*> const*>),
      i_(0) {}

  φc_(Sp<Stc> x__, Sp<M<φmk_, φ_<void*> const*>> m__, uN i__)
    : x_(x__), m_(m__), i_(i__) {}

  φc_ at(uN j) const noexcept { return φc_{x_, m_, j}; }

  template<class T> φr_<T> a(T     const &y, uN j) const noexcept { return φr_<T>{i_, j, y}; }
  template<class T> φr_<T> f(φ_<T> const *p, uN j) const noexcept { return φr_<T>{i_, j, p}; }

  Stc &x() const noexcept { return *x_; }
  uN   i() const noexcept { return i_; }
  uN   l() const noexcept { return x_->size() - i_; }
  uN   n() const noexcept { return x_->size(); }

  ch operator[](iN i) const noexcept { return x_->at(i_ + i); }
  St sub(uN n)        const noexcept { return {x_->data() + i_, n}; }
  St sub(uN s, uN n)  const noexcept { return {x_->data() + i_ + s, n}; }

  bool sw(Stc &x) const noexcept
    { return x.size() <= l() && !memcmp(x_->data() + i_, x.data(), x.size()); }

  St::const_iterator it(uN i) const noexcept { return x_->begin() + i_ + i; }

  template<class T>
  M<φmk_, φ_<T> const*> &m() const noexcept
    { return *Rc<M<φmk_, φ_<T> const*>*>(m_.get()); }


protected:
  Sp<Stc>                       x_;  // input
  Sp<M<φmk_, φ_<void*> const*>> m_;  // cache of parse failures
  uN                            i_;  // input offset
};


template<class T>
struct φ final
{
  φ()          :        id(φgen_id()) {}
  φ(φ_<T> *p_) : p(p_), id(φgen_id()) {}

  φ &operator=(φ<T> const &x) { p = x.p; id = x.id; return *this; }

  operator bool() const { return Sc<bool>(p); }

  φr_<T> operator()(Stc       &x) const noexcept { return (*this)(φc_(x)); }
  φr_<T> operator()(φc_ const &x) const noexcept
    { let   k = φmk(id, x.i());
      auto &m = x.template m<T>();
      if (m.contains(k)) return x.f(m.at(k), x.i());
      let r = (*p)(x);
      if (r.is_f()) m[k] = &r.p();
      return mo(r); }


  St name() const noexcept { return (Ss{} << id << ":" << p->name()).str(); }

  template<class X>
  X &as() const noexcept { return *dynamic_cast<X*>(p.get()); }

  φ_<T> &operator*() const noexcept { return *p; }

  Sp<φ_<T>> p;
  φid_      id;
};


template<class T> O &operator<<(O &s, φ_<T> const &f) { return s << f.name(); }
template<class T> O &operator<<(O &s, φ <T> const &f) { return s << f.name(); }


}

#include "end.hh"


#endif
