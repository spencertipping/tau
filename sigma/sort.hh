#ifndef τshare_sort_h
#define τshare_sort_h

#include <algorithm>
#include <cstdlib>

#include "../Gamma.hh"

#include "../begin.hh"

namespace τ
{


// Sort current ξ on τ boundaries, using natural η ordering
Γ Γsort(uN bufsize  = 64 * 1048576,
        St tmp      = "/tmp/tau-sort-XXXXXXXX",
        Γ  disk_enc = nullptr,
        Γ  disk_dec = nullptr);


// Merge many sorted ξs
// TODO: use Ξ channels, not Γ arguments
Γ Γmerge(Vc<Γ>&);


}

#include "../end.hh"


#endif
