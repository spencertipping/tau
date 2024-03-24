#ifndef σplatform_h
#define σplatform_h

#include "../sigma-pre.hh"
#include "begin.hh"

namespace σ
{


void Γnative(Γφ&);  // platform-specific, defined in linux/ and wasm/
void Γshared(Γφ&);  // cross-platform

void πnative(πφ&);
void πshared(πφ&);


}

#include "end.hh"

#endif
