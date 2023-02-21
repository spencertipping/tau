#ifndef τshare_mux_h
#define τshare_mux_h


#include "../gamma.hh"
#include "../begin.hh"

namespace τ
{


// Collapse all named duplexes down to a single primary channel. Fanout is
// on the left, single channel is on the right.
γ γmux();


// Expand primary channel into named duplexes, each with either a
// special-case γ or a fallback. Fanout is on the right, single channel
// on the left.
γ γdemux(M<St, γ>&, γ);


}

#include "../end.hh"


#endif
