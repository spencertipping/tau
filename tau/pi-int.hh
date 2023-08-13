#ifndef τπint_h
#define τπint_h

#include "xiio.hh"
#include "tau.hh"
#include "pi-heap.hh"
#include "begin.hh"

namespace τ
{


// π interpreter registers
struct πir final : public virtual πhv
{
  πir(πh &h) : πhv(h)
    { x_.reserve(8); y_.reserve(8); push_x({}); push_y({}); }

  πhr &x() { return x_.back(); }
  πhr &y() { return y_.back(); }

  πir &push_x(πhr const &x) { x_.push_back(x); return *this; }
  πir &push_y(πhr const &y) { y_.push_back(y); return *this; }
  πhr  pop_x()              { let r = x_.back(); x_.pop_back(); return r; }
  πhr  pop_y()              { let r = y_.back(); y_.pop_back(); return r; }

  void mark() { for (let  &v : x_)     h.mark(v); for (let  &v : y_)     h.mark(v); }
  void move() { for (auto &v : x_) v = h.move(v); for (auto &v : y_) v = h.move(v); }

protected:
  V<πhr> x_;
  V<πhr> y_;
};


// π external (C++) values
struct πev
{
  virtual ~πev() = default;
};


// π program interpreter
struct πi final
{
  πi()      : t_(nullptr) {}
  πi(τe &t) : t_(&t) {}

  πi(τe &t, ξi fi)                      : t_(&t), fi_(fi) {}
  πi(τe &t, ξo fo)                      : t_(&t), fo_(fo) {}
  πi(τe &t, ξi fi, ξo fo)               : t_(&t), fi_(fi), fo_(fo) {}
  πi(τe &t, ξi fi, ξo fo, ξo bo, ξi bi) : t_(&t), fi_(fi), fo_(fo), bo_(bo), bi_(bi) {}


  τe *t() const { return t_; }


  πi &push(πhr const &r) { s_.xs.push_back(r);       return *this; }
  πi &push(ηi  const &i) { s_.xs.push_back(h_ << i); return *this; }
  πi &push(πhr const &r, ηic &i)
    { s_.xs.push_back(h_.i(r, i)); return *this; }

  πhr peek() const
    { A(!s_.xs.empty(), "πi stack underflow (peek)");
      return s_.xs.back(); }

  πhr pop()
    { A(!s_.xs.empty(), "πi stack underflow (pop)");
      let r = s_.xs.back(); s_.xs.pop_back(); return r; }

  πi &dup()  { return push(peek()); }
  πi &drop() { pop(); return *this; }
  πi &swap()
    { A(s_.xs.size() >= 2, "πi stack underflow (swap)");
      std::swap(s_.xs.back(), s_.xs[s_.xs.size() - 2]); return *this; }

  uN               size()     const { return s_.xs.size(); }
  bool            empty()     const { return s_.xs.empty(); }
  πhr const &operator[](uN i) const { return s_.xs[size() - i - 1]; }
  πhr       &operator[](uN i)       { return s_.xs[size() - i - 1]; }


  ηi ypeek() const { return (*this)[peek()]; }
  ηi ypop()        { return (*this)[pop()]; }

  ηi operator[](πhr const &r)         const { return h_[r]; }
  πhr         i(πhr const &r, ηic &i) const { return h_.i(r, i); }

  template<ηauto_encode T>
  πhr operator<<(T const &x) { return h_ << x; }


  ηo<πh&> r(uN s = 64)                         { return h_.r(s); }
  πhr   ref()                                  { return h_.ref(); }
  πhr     r(uN s, F<void(ηo<πh&>&&)> const &f) { f(r(s)); return ref(); }

  πh &h() { return h_; }


  πhr  mg(Stc &n)         const { return m_.xs.at(n); }
  bool mi(Stc &n)         const { return m_.xs.contains(n); }
  πi  &ms(Stc &n, πhr const &x) { m_.xs[n] = x;   return *this; }
  πi  &mx(Stc &n)               { m_.xs.erase(n); return *this; }


  ξi &fi() { return fi_; }
  ξo &fo() { return fo_; }
  ξo &bo() { return bo_; }
  ξi &bi() { return bi_; }

  πhr &x() { return r_.x(); }
  πhr &y() { return r_.y(); }

  πi &def_x()             { r_.x() = pop(); return *this; }
  πi &def_y()             { r_.y() = pop(); return *this; }
  πi &def_x(πhr const &r) { r_.x() = r;     return *this; }
  πi &def_y(πhr const &r) { r_.y() = r;     return *this; }

  πi &save_x()    { r_.push_x(r_.x()); return *this; }
  πi &save_y()    { r_.push_y(r_.y()); return *this; }
  πi &restore_x() { return push(r_.pop_x()); }
  πi &restore_y() { return push(r_.pop_y()); }

  πi &clear_x()  { r_.x().clear(); return *this; }
  πi &clear_y()  { r_.y().clear(); return *this; }
  πi &clear_xy() { clear_x(); return clear_y(); }

  Tt πi   &def_ev(Stc &n, T    *v) { e_[n] = Sp<πev>(v);           return *this; }
  Tt πi   &def_ev(Stc &n, Sp<T> v) { e_[n] = v.template as<πev>(); return *this; }
  Tt Sp<T> ev    (Stc &n)    const { return e_.at(n).template as<T>(); }
  Tt bool  evi   (Stc &n)    const { return e_.contains(n); }
  Tt πi   &evx   (Stc &n)          { e_.erase(n); return *this; }


protected:
  τe  *t_;      // event loop (for sub-Γ evaluation)
  πh   h_;
  πhsv s_{h_};  // data stack
  πhmv m_{h_};  // named bindings
  πir  r_{h_};

  M<St, Sp<πev>> e_;  // external values

  ξi fi_;
  ξo fo_;
  ξo bo_;
  ξi bi_;

  friend O &operator<<(O&, πi const&);
};


O &operator<<(O&, πi const&);


}

#include "end.hh"

#endif
