#ifndef τπ_h
#define τπ_h


#include "pi-types.hh"
#include "pi-gc.hh"
#include "pi-abi.hh"
#include "pi-pgm.hh"
#include "pi-int.hh"
#include "pi-asm.hh"

#include "pi-abi1.hh"

#if τplatform == τplatform_linux
# include "pi-abi2-linux.hh"
#elif τplatform == τplatform_wasm
# include "pi-abi2-wasm.hh"
#endif


#endif
