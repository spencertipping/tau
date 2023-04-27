#include "psi4.hh"
#include "begin.hh"

namespace σ
{

using namespace τ;


void http_req_loop(ξi i, ξo o);
void http_res_loop(ξi i, ξo o);


φ<Ψ4> φΨ4()
{
  static φ<Ψ4> r = φL<Ψ4>([]()
    { return φd<Ψ4>("Ψ4",
                    "H", φauto(Γhttp)); });
  return r;
}


}

#include "end.hh"
