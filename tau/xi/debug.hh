#ifndef τξdebug_h
#define τξdebug_h


#include "begin.hh"

namespace τ::ξ
{


ϝ &stream_out(Φ &f, O &s)
{
  return *new ϝ(f, ϝ::ξΦ, [&](ϝ &f)
    { for (let x : f) s << f.g.f.dt() << ": " << x << std::endl; });
}


}

#include "end.hh"


#endif
