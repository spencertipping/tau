#ifndef τ²emscripten_h
#define τ²emscripten_h


#include "../arch.hh"

#include <stdio.h>

#if τplatform == τplatform_wasm
# include <emscripten/emscripten.h>
# include <emscripten/fiber.h>
# include <emscripten/websocket.h>
#else
# warning τemscripten: emulating headers and EM_ macros
# define τassume_emscripten 1
# define EM_ASM(...)
# define EM_JS(...)
// Enough definitions to enable clang/LSP to understand the emscripten
// bindings
  void emscripten_async_call(void(*)(void*), void*, int);
#endif


#include "../types.hh"

#include "../lambda.hh"
#include "../Lambda.hh"

#include "../gate.hh"
#include "../tau-common.hh"


#include "../begin.hh"

namespace τ
{


struct τe;

void τset(τe*);
void τstep(void*);  // invoked by callbacks to advance τ


struct τe : public τb
{
  τe(τe&)  = delete;
  τe(τe&&) = delete;
  τe () { τset(this); }
  ~τe() { τset(nullptr); }


  explicit operator bool() const;

  τe &wake();
  τe &schedule();
  τe &operator()();

  τe &go(F<bool(τe&)> &&f = [](τe &t) { return Sc<bool>(t); });

  bool should_step();


protected:
  F<bool(τe&)> go_f;
  uN           nts  = 0;  // number of tracked things
};


}

#include "../end.hh"


#endif
