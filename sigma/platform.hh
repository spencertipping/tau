#ifndef σplatform_h
#define σplatform_h

#include "../sigma-pre.hh"

#if τlinux
# include "linux/lmdb.hh"
# include "linux/lmdb-index.hh"
# include "linux/lmdb-sstream.hh"
# include "linux/Omega.hh"
#endif


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
