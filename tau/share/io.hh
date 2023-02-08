#ifndef τshare_io_h
#define τshare_io_h

#include "../gamma.hh"

#include "../begin.hh"

namespace τ
{


// Write values into ostream, copying to output; this is tappable and allows
// custom prefixes
Sp<γ> γostream(O&, St = "", bool = false);


}

#include "../end.hh"

#endif
