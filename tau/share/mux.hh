#ifndef τshare_mux_h
#define τshare_mux_h


#include "../gamma.hh"
#include "../begin.hh"

namespace τ
{


// TODO: how do we define IO for these?
// Should we use Ξ channels? (Probably.)

γ γmux  (M<St, γ>&);
γ γdemux(M<St, γ>&);


}

#include "../end.hh"


#endif
