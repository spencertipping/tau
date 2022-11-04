#ifndef τΦ_emscripten_h
#define τΦ_emscripten_h


#include "arch.hh"

#if τplatform == τplatform_wasm
# include <emscripten.h>
#else
# define EM_ASM(x)
# define EM_JS(...)
#endif


void foo()
{
  EM_ASM("alert('hi');");
}


#endif
