#ifndef σcontainer_h
#define σcontainer_h

#include "../sigma-pre.hh"
#include "begin.hh"

namespace σ
{

Γ container(pre::ctype, pre::cback, τ::Ψd = τ::Ψd::f);
Γ container_ls(pre::cback, τ::Ψd = τ::Ψd::f);

void Γcontainer(Γφ&);

}

#include "end.hh"

#endif
