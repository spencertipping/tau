#ifndef τshare_io_h
#define τshare_io_h

#include "../Gamma.hh"
#include "../strings.hh"

#include "../begin.hh"

namespace τ
{


// Write values into ostream, copying to output; this is tappable and allows
// custom prefixes
Γ Γostream(O&, St = "", bool = false);


// Compress each η with zstd
Γ ΓZ(u8 l = 3);


}

#include "../end.hh"

#endif
