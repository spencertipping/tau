#ifndef τlinux_io_h
#define τlinux_io_h


#include "../Gamma.hh"
#include "../tau.hh"

#include "../begin.hh"

namespace τ
{


Γ Γfr(fd_t, bool b = false);
Γ Γfw(fd_t, bool b = false);

Γ Γfcat(bool τ = false);  // == ni \<


}

#include "../end.hh"


#endif
