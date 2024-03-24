#include <fstream>
#include <unistd.h>


#include "../../sigma.hh"

#if τgl
# include "gl.hh"
#endif

#include "http.hh"
#include "io.hh"
#include "lmdb.hh"
#include "zstd.hh"

#include "../begin.hh"

namespace σ
{

using namespace τ;


void Γnative(Γφ &g)
{
#if τgl
  Γlinux_gl(g);
#endif

  Γhttp(g);
  Γio(g);
  Γlmdb(g);
  Γzstd(g);

  // Compile-time file IO
  g.def_g("$<", [&](Stc &f, Ξc &x)
    { std::ifstream f_(f);
      A(f_, "could not open file " << f);
      let s = St{std::istreambuf_iterator<char>(f_), std::istreambuf_iterator<char>()};
      let r = g.parse(s);
      A(r.is_a(), "could not parse file " << f << ": error at position " << r.j << ", here: -->|" << s.substr(r.j) << "|<--");
      return r.r()(x); });
}


void πnative(πφ &p)
{
  p .def_sl("uP", []() { return i64(getpid()); })
    .def_sl("$<", [](πslc<St> &f)
      { std::ifstream f_(f.x);
        A(f_, "could not open file " << f.x);
        return St{std::istreambuf_iterator<char>(f_), std::istreambuf_iterator<char>()}; });
}


}

#include "../end.hh"
