#ifndef τηm_h
#define τηm_h

#include "eta-i.hh"
#include "eta-o.hh"

#include "begin.hh"

namespace τ
{


struct ηm;
typedef ηm const ηmc;


// η memory buffer: store an ηi's datastream as a C++ object
struct ηm final
{
  ηm()     = default;
  ηm(ηm&)  = default;
  ηm(ηm&&) = default;
  ηm(ηic &x) { *this = x; }

  ηm &operator=(ηic &x)
    { x_.assign(x.odata(), x.osize() + x.asize()); return *this; }

  operator ηi() const { return y(); }
  ηi        y() const { return ηi(x_.data(), x_.size()); }

  ηm  &clear()       { x_.clear(); return *this; }
  bool empty() const {             return x_.empty(); }

  PO operator<=>(ηic &x) const { return y() <=> x; }
  PO operator<=>(ηmc &x) const { return y() <=> x.y(); }

  template<class T>
  ηm &operator<<(T const &x) { ηo{ηoc<B&>(x_)} << x; return *this; }

  B x_;
};


template<class T>
ηo<T> &operator<<(ηo<T> &x, ηmc &y)
{
  return x << Sn<u8c>{y.x_.data(), y.x_.size()};
}


}

#include "end.hh"

#endif
