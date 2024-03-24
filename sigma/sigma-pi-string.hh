#ifndef σstring_h
#define σstring_h

#include "../sigma-pre.hh"
#include "begin.hh"

namespace σ
{

τ::St η_to_json(τ::ηic&);
τ::ηm json_to_η(τ::Stc&);

τ::St hex_decode(τ::Stc&);
τ::St hex_encode(τ::Stc&);

void πstring(πφ&);

}

#include "end.hh"

#endif
