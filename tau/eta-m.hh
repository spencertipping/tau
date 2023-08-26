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
  ηm(ηmc&) = default;
  ηm(ηm&&) = default;
  ηm(ηic &x) { *this = x; }
  ηm(uN c)   { reserve(c); }

  ηm &operator=(ηic &x)
    { x_.assign(x.odata(), x.osize() + x.asize()); return *this; }

  ηm &operator=(ηm&&) = default;
  ηm &operator=(ηmc&) = default;

  operator ηi() const { return y(); }
  ηi        y() const { return ηi(x_.data(), x_.size()); }
  Sn<u8c> all() const { return Sn<u8c>{x_.data(), x_.size()}; }
  uN    lsize() const { return x_.size(); }

  ηm  &clear()       { x_.clear();    return *this; }
  ηm  &reserve(uN s) { x_.reserve(s); return *this; }
  bool empty() const {                return x_.empty(); }

  PO   operator<=>(ηic &x) const { return y() <=> x; }
  PO   operator<=>(ηmc &x) const { return y() <=> x.y(); }
  bool operator== (ηmc &x) const { return x_  ==  x.x_; }

  Tt ηm &operator<<(T const &x) { ηo{ηoc<B&>(x_)} << x; return *this; }

  B x_;
};


Tt ηo<T> &operator<<(ηo<T> &x, ηmc &y)
{
  return x << Sn<u8c>{y.x_.data(), y.x_.size()};
}


O &operator<<(O&, ηmc&);


}


namespace std
{

Tn struct hash<τ::ηm>
{
  τ::uS operator()(τ::ηmc &x) const;
};

}


#include "end.hh"

#endif
