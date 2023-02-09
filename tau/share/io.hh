#ifndef τshare_io_h
#define τshare_io_h

#include "../gamma.hh"

#include "../begin.hh"

namespace τ
{


// Write values into ostream, copying to output; this is tappable and allows
// custom prefixes
Sp<γ> γostream(O&, St = "", bool = false);


// Split TSV into tuples
// TODO: split on any delimiter, either into τ or tuple
Sp<γ> γtsv();


}

#include "../end.hh"

#endif
