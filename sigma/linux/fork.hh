#ifndef σlinux_fork_h
#define σlinux_fork_h

#include <unistd.h>

#include "../begin.hh"

namespace σ
{

using namespace τ;


Ξ τfork2(Γa<Γ>, Ξc&);
Ξ τfork4(Γa<Γ>, Ξc&);
Ξ fork_exec(Vc<St>&, Ξc&);

void Γfork(Γφ&);

}

#include "../end.hh"

#endif
