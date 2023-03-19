#include "toplevel.hh"
#include "begin.hh"

namespace σ
{


static σφd<τ::πfn> φp;
static σφd<σlens>  φl;
static σφd<τ::Γ>   φΓ;
static σφd<τ::Γp>  φΓp;


// TODO: initialize here

σφd<τ::πfn> &σφp()  { return φp;  }
σφd<σlens>  &σφl()  { return φl;  }
σφd<τ::Γ>   &σφΓ()  { return φΓ;  }
σφd<τ::Γp>  &σφΓp() { return φΓp; }


}

#include "end.hh"
