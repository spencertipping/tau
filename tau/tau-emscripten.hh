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


// TODO: possibly rework this logic; it's less bad than epoll but
// may still need to be fixed


template<class O>
struct τf  // boundary FD-like wrapper, e.g. websocket or GL context
{
  τ  &f;
  λg  w;
  O   o;

  τf(τf const&) = delete;
  τf(τf &&)     = delete;

  template<class... T> τf(τ&, T...);
  ~τf();

  void init();
};


τ *τg = nullptr;

void τstep(void*);


struct τ : public τb
{
  F<bool(τ&)> go_f;
  uN          nts  = 0;  // number of tracked things

  τ(τ&)  = delete;
  τ(τ&&) = delete;
  τ () { A(!τg, "cannot define multiple τs in wasm"); τg = this; }
  ~τ() { τg = nullptr; }

  constexpr bool is_async() { return true; }


  operator bool() const { return nts || hn() != forever(); }

  τ &wake()
    { while (now() >= hn()) l.r(h.top().l), h.pop();
      return *this; }

  τ &schedule()
    { let t = now();
      if (hn() < forever())
        if (t < hn())
        { let dt = (hn() - t) / 1ms;
          emscripten_async_call(τstep, this, std::min(dt, Sc<decltype(dt)>(Nl<int>::max()))); }
        else
          emscripten_async_call(τstep, this, 0);
      return *this; }

  τ &operator()()
    { wake();
      l.go();
      return schedule(); }


  τ &go(F<bool(τ&)> const& = [](τ&) { return true; })
    { A(0, "τ is async"); return *this; }

  τ &go_async(F<bool(τ&)> &&f = [](τ &f) { return Sc<bool>(f); })
    { go_f = std::move(f);
      l.go();
      emscripten_async_call(τstep, this, 0);
      return *this; }
};


void τstep(void *f_)  // invoked by callbacks to advance τ
{
  τ &f = *Rc<τ*>(f_);
  if (f.go_f(f)) f();
}


}

#include "end.hh"


#endif
