#ifndef σstring_h
#define σstring_h

#include "../sigma.hh"
#include "begin.hh"

namespace σ
{

using namespace τ;

St η_to_json(ηic&);
ηm json_to_η(Stc&);

St hex_decode(Stc&);
St hex_encode(Stc&);

void πstring(πφ&);

}

#include "end.hh"

#endif
