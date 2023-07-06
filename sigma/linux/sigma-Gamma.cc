#include <unistd.h>

#include "../../sigma.hh"
#include "fork.hh"
#include "http.hh"
#include "io.hh"

#include "../begin.hh"

namespace σ
{

using namespace τ;


void Γnative(Γφ &g)
{
  Γfork(g);
  Γio(g);
  Γhttp(g);
}


void πnative(πφ &p)
{
  p .def_sa([](φaL<'u', 'P'>) { return π1{"getpid", [](πi &i) { i.push(i << i64(getpid())); }}; });
}


}

#include "../end.hh"
