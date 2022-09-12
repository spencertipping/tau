#ifndef τλ_h
#define τλ_h


#include "arch.hh"
#include "lambda-class.hh"

#if τplatform == τplatform_wasm
# include "lambda-emscripten.hh"
#elif τarch == τarch_linux64
# include "lambda-boost.hh"
#else
# error λ not supported on this architecture
#endif


#ifdef τassume_emscripten
# error τassume_emscripten is not intended for production
#endif


#endif
