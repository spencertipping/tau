#include "../tau.hh"
#include "begin.hh"

namespace τ
{


O &operator<<(O &s, πi const &v)
{
  s << "πi |s|=" << v.s_.xs.size() << " |m|=" << v.m_.xs.size() << std::endl;
  s << "  h=" << v.h_ << std::endl;
  s << "  s="; for (let &x      : v.s_.xs) s <<             x << "(" << v[x] << ") "; s << std::endl;
  s << "  m="; for (let &[k, x] : v.m_.xs) s << k << ":" << x << "(" << v[x] << ") "; s << std::endl;
  return s;
}


}

#include "end.hh"
