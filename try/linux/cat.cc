#include "../../tau.hh"
#include "../../sigma.hh"

#include "../../tau/begin.hh"

using namespace std;
using namespace σ;


int main()
{
  τassert_begin
  τ::τe t;
  τ::φE(φΓ())("<F0>F1").r()(τ::Ξ{t}.push(τ::ξdc{}));
  t.go();
  A(!τ::ξn(), "ξn = " << τ::ξn());
  A(!τ::ψn(), "ψn = " << τ::ψn());
  return 0;
  τassert_end
}


#include "../../tau/end.hh"
