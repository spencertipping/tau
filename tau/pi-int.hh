#ifndef τπint_h
#define τπint_h

#include "xiio.hh"
#include "pi-heap.hh"
#include "begin.hh"

namespace τ
{


// π program interpreter
struct πi final
{
  πi()          = default;
  πi(πi const&) = default;
  πi(πi&&)      = default;

  πi(ξi fi)                      : fi_(fi) {}
  πi(ξo fo)                      : fo_(fo) {}
  πi(ξi fi, ξo fo)               : fi_(fi), fo_(fo) {}
  πi(ξi fi, ξo fo, ξo bo, ξi bi) : fi_(fi), fo_(fo), bo_(bo), bi_(bi) {}


  πi &push(πhr const &r) { s_.xs.push_back(r);       return *this; }
  πi &push(ηi  const &i) { s_.xs.push_back(h_ << i); return *this; }
  πi &push(πhr const &r, ηi const &i)
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
  πhr const &operator[](uN i) const { return s_.xs[size() - i - 1]; }
  πhr       &operator[](uN i)       { return s_.xs[size() - i - 1]; }


  ηi ypeek() const { return (*this)[peek()]; }
  ηi ypop()        { return (*this)[pop()]; }

  ηi operator[](πhr const &r)              const { return h_[r]; }
  πhr         i(πhr const &r, ηi const &i) const { return h_.i(r, i); }

  template<ηauto_encode T> πhr operator<<(T const &x) { return h_ << x; }


  ηo<πh&> r(uN s = 64)                       { return h_.r(s); }
  πhr   ref()                                { return h_.ref(); }
  πhr     r(uN s, F<void(ηo<πh&>)> const &f) { f(r(s)); return ref(); }

  πh &h() { return h_; }


  πhr  mg(Stc &n)         const { return m_.xs.at(n); }
  bool mi(Stc &n)         const { return m_.xs.contains(n); }
  πi  &ms(Stc &n, πhr const &x) { m_.xs[n] = x;   return *this; }
  πi  &mx(Stc &n)               { m_.xs.erase(n); return *this; }


  ξi &fi() { return fi_; }
  ξo &fo() { return fo_; }
  ξo &bo() { return bo_; }
  ξi &bi() { return bi_; }


protected:
  πh   h_;
  πhsv s_{h_};  // data stack
  πhmv m_{h_};  // named bindings

  ξi   fi_;
  ξo   fo_;
  ξo   bo_;
  ξi   bi_;

  friend O &operator<<(O&, πi const&);
};


O &operator<<(O&, πi const&);


}

#include "end.hh"

#endif
