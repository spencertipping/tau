#ifndef τΦ_h
#define τΦ_h


#include "arch.hh"

#if τhas_epoll
# include "Phi-epoll.hh"
#elif τplatform == τplatform_wasm
# include "Phi-emscripten.hh"
#endif


#endif
