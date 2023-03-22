#ifndef τshare_mux_h
#define τshare_mux_h


#include "../Gamma.hh"
#include "../begin.hh"

namespace τ
{


// Collapse all named duplexes down to a single primary channel. Fanout is
// on the left, single channel is on the right.
Γ Γmux();


// Expand primary channel into named duplexes, each with either a
// special-case Γ or a fallback. Fanout is on the right, single channel
// on the left.
Γ Γdemux(M<St, Γ>&, Γ);


}

#include "../end.hh"


#endif
