#ifndef τπ0_h
#define τπ0_h


#include "pi0-types.hh"
#include "pi0-gc.hh"
#include "pi0-abi.hh"
#include "pi0-pgm.hh"
#include "pi0-int.hh"
#include "pi0-asm.hh"

#include "pi0-abi1.hh"

#if τplatform == τplatform_linux
# include "pi0-abi2-linux.hh"
#elif τplatform == τplatform_wasm
# include "pi0-abi2-wasm.hh"
#endif


#endif
