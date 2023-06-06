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
  πi &push(πhr const &r) { s.xs.push_back(r);      return *this; }
  πi &push(ηi  const &i) { s.xs.push_back(h << i); return *this; }
  πi &push(πhr const &r, ηi const &i)
    { s.xs.push_back(h.i(r, i)); return *this; }

  πhr peek() const { return s.xs.back(); }
  πhr pop()        { let r = s.xs.back(); s.xs.pop_back(); return r; }

  ηi ypeek() const { return (*this)[peek()]; }
  ηi ypop()        { return (*this)[pop()]; }

  ηi operator[](πhr const &r) const { return h[r]; }

protected:
  πh   h;
  πhsv s{h};  // data stack
  πhmv m{h};  // named bindings
};


}

#include "end.hh"

#endif
