#ifndef τξhttp1_h
#define τξhttp1_h


#include "begin.hh"

namespace τ::ξ
{


ϝ &http1(Φ &f, uN l = 65536)
{
  return *new ϝ(f, "http1", ϝ::ξϊ, [&, l]()
    { B b; b.reserve(l);

      // TODO: parse HTTP/1 request
      // TODO: create HTTP/1 reply (separate λ)
    });
}


}

#include "end.hh"


#endif
