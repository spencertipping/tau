#include <algorithm>

#define XXH_INLINE_ALL 1
#include <xxhash.h>

#include "eta-m.hh"
#include "begin.hh"

namespace τ
{


ηm ηm::sorted() const
{
  V<ηi> xs;
  for (let &x : y()) xs.push_back(x);
  std::sort(xs.begin(), xs.end());
  ηm r{lsize()};
  for (let &x : xs) r << x;
  return r;
}


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
