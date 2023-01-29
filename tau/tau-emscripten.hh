#ifndef τ²emscripten_h
#define τ²emscripten_h


#include "arch.hh"

#include <stdio.h>

#if τplatform == τplatform_wasm
# include <emscripten/emscripten.h>
# include <emscripten/fiber.h>
# include <emscripten/websocket.h>
#else
# warning τemscripten: emulating headers and EM_ macros
# define EM_ASM(...)
# define EM_JS(...)
// Enough definitions to enable clang/LSP to understand the emscripten
// bindings
  void emscripten_async_call(void(*)(void*), void*, int);
#endif


#include "types.hh"

#define τassume_emscripten 1
# include "lambda.hh"
# include "Lambda.hh"
#undef τassume_emscripten

#include "gate.hh"
#include "tau-common.hh"


#include "begin.hh"

namespace τ
{


struct τe;
τe *τg = nullptr;

void τstep(void*);


struct τe : public τb
{
  τe(τe&)  = delete;
  τe(τe&&) = delete;
  τe () { A(!τg, "cannot define multiple τs in wasm"); τg = this; }
  ~τe() { τg = nullptr; }


  operator bool() const { return nts || hn() != forever(); }

  τe &wake()
    { while (now() >= hn()) l.r(h.top().l), h.pop();
      return *this; }

  τe &schedule()
    { let t = now();
      if (hn() < forever())
        if (t < hn())
        { let dt = (hn() - t) / 1ms;
          emscripten_async_call(τstep, this, std::min(dt, Sc<decltype(dt)>(Nl<int>::max()))); }
        else
          emscripten_async_call(τstep, this, 0);
      return *this; }

  τe &operator()()
    { wake();
      l.go();
      return schedule(); }


  τe &go(F<bool(τe&)> const& = [](τe&) { return true; })
    { A(0, "τ is async"); return *this; }

  τe &go_async(F<bool(τe&)> &&f = [](τe &t) { return Sc<bool>(t); })
    { go_f = std::move(f);
      l.go();
      emscripten_async_call(τstep, this, 0);
      return *this; }


  bool should_step() { return go_f(*this); }


protected:
  F<bool(τe&)> go_f;
  uN           nts  = 0;  // number of tracked things
};


void τstep(void *t_)  // invoked by callbacks to advance τ
{
  τe &t = *Rc<τe*>(t_);
  if (t.should_step()) t();
}


}

#include "end.hh"


#endif
