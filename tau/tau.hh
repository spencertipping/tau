#ifndef τ²_h
#define τ²_h


#include "arch.hh"

#if τhas_epoll
# include "tau-epoll.hh"
#elif τplatform == τplatform_wasm
# include "tau-emscripten.hh"
#endif


#endif
