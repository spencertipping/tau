#ifndef τshare_io_h
#define τshare_io_h

#include "../gamma.hh"
#include "../strings.hh"

#include "../begin.hh"

namespace τ
{


// Write values into ostream, copying to output; this is tappable and allows
// custom prefixes
γ γostream(O&, St = "", bool = false);


// Compress each η with zstd
γ γZ(u8 l = 3);


}

#include "../end.hh"

#endif
