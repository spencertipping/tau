#include <fstream>
#include <unistd.h>

#include "../../sigma.hh"
#include "fork.hh"
#include "http.hh"
#include "io.hh"
#include "zstd.hh"

#include "gl.hh"

#include "../begin.hh"

namespace σ
{

using namespace τ;


void Γnative(Γφ &g)
{
  Γfork(g);
  Γio(g);
  Γhttp(g);
  Γlinux_gl(g);
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
  p .def_sl("uP", []() { return i64(getpid()); });
}


}

#include "../end.hh"
