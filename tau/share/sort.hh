#ifndef τshare_sort_h
#define τshare_sort_h

#include <algorithm>
#include <cstdlib>

#include "../gamma.hh"

#include "../begin.hh"

namespace τ
{


// Sort current ξ on τ boundaries, using natural η ordering
γ γsort(uN bufsize  = 64 * 1048576,
        St tmp      = "/tmp/tau-sort-XXXXXXXX",
        γ  disk_enc = nullptr,
        γ  disk_dec = nullptr);


// Merge many sorted ξs
// TODO: use Ξ channels, not γ arguments
γ γmerge(Vc<γ>&);


}

#include "../end.hh"


#endif
