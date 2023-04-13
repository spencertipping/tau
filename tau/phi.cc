#include "phi.hh"
#include "begin.hh"

namespace τ
{


static φid_ φid__ = 0;
φid_ φgen_id() { return ++φid__; }


}

#include "end.hh"
