#include "../../sigma.hh"
#include "http.hh"
#include "io.hh"

#include "../begin.hh"

namespace σ
{

using namespace τ;


void Γnative(Γφ &g)
{
  Γio(g);
  Γhttp(g);
}


void πnative(πφ &p)
{
}


}

#include "../end.hh"
