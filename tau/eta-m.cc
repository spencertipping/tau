#define XXH_INLINE_ALL 1
#include <xxhash.h>

#include "eta-m.hh"
#include "begin.hh"

namespace τ
{

ηm operator+(ηic &x, ηic &y)
{
  return ηm{x.lsize() + y.lsize()} << x << y;
}

O &operator<<(O &s, ηmc &v)
{
  return s << v.y();
}

}

namespace std
{

τ::uS hash<τ::ηm>::operator()(τ::ηmc const &x) const
{
  return τ::uN(XXH64(x.x_.data(), x.x_.size(), 0));
}

}

#include "end.hh"
