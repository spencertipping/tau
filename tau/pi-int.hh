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
  πi &push(πhr const &r) { s_.xs.push_back(r);       return *this; }
  πi &push(ηi  const &i) { s_.xs.push_back(h_ << i); return *this; }
  πi &push(πhr const &r, ηi const &i)
    { s_.xs.push_back(h_.i(r, i)); return *this; }

  πhr peek() const { return s_.xs.back(); }
  πhr pop()        { let r = s_.xs.back(); s_.xs.pop_back(); return r; }

  ηi ypeek() const { return (*this)[peek()]; }
  ηi ypop()        { return (*this)[pop()]; }

  ηi operator[](πhr const &r)              const { return h_[r]; }
  πhr         i(πhr const &r, ηi const &i) const { return h_.i(r, i); }

  Tt πhr operator<<(T const &x) { return h_ << x; }

  πh &h() { return h_; }

protected:
  πh   h_;
  πhsv s_{h_};  // data stack
  πhmv m_{h_};  // named bindings
};


}

#include "end.hh"

#endif
