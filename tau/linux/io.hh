#ifndef τlinux_io_h
#define τlinux_io_h


#include "../gamma.hh"
#include "../tau.hh"

#include "../begin.hh"

namespace τ
{


Sp<γ> γfr(fd_t);
Sp<γ> γfw(fd_t);

Sp<γ> γfcat(bool τ = false);  // == ni \<


}

#include "../end.hh"


#endif
