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


γ γsplit_chr(cs7);
γ γsplit_str(St);


}

#include "../end.hh"

#endif
