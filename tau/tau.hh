#ifndef τ²_h
#define τ²_h


#include "arch.hh"

#if τhas_epoll
# include "linux/tau-epoll.hh"
#elif τplatform == τplatform_wasm
# include "wasm/tau-emscripten.hh"
#else
# error no τ² implementation for this platform
#endif


#endif
