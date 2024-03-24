#ifndef σlinux_io_h
#define σlinux_io_h

#include "../../sigma.hh"
#include "../begin.hh"

namespace σ
{

using namespace τ;


Γ ΓwF(fd_t, Ψd = Ψd::f);
Γ ΓrF(fd_t, Ψd = Ψd::f);
Γ ΓT (i64, i64, Γ, Ψd = Ψd::f);

void Γio(Γφ&);


}

#include "../end.hh"

#endif
