#include "sigma-Gamma-pieces.hh"
#include "begin.hh"

namespace σ
{

using namespace std::literals;
using namespace τ;


void πshared_π(πφ &p)
{
  p
#if τhas_threads
    .def_sl("&&|", [](πi &i, πp<π1> f) { return (*i.t())([&]() { f.x(i); return i.pop(); }); })
#endif
    .def_sl("dΘ|", [](πi &i, πp<π1> f)
      {
        let t1 = now();
        f.x(i);
        let t2 = now();
        i.r(i.ypeek().lsize() + 8, [&](auto &&o)
          {
            o << (t2 - t1) / Θq;
            o << i.ypeek().all();
          });
        i.swap();
        i.drop();
        return i.pop();
      });
}


}

#include "end.hh"
