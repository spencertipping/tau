#ifndef τΦemscripten_h
#define τΦemscripten_h


#include "arch.hh"

#include <stdio.h>

#if τplatform == τplatform_wasm
# include <emscripten/emscripten.h>
# include <emscripten/fiber.h>
# include <emscripten/websocket.h>
#else
# warning Φemscripten: emulating headers and EM_ macros
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

#include "phi.hh"
#include "utf9.hh"

#include "Phi-common.hh"


#include "begin.hh"

namespace τ
{


template<class O>
struct Φf  // boundary FD-like wrapper, e.g. websocket or GL context
{
  Φ  &f;
  λg  w;
  O   o;

  Φf(Φf const&) = delete;
  Φf(Φf &&)     = delete;

  template<class... T> Φf(Φ&, T...);
  ~Φf();

  void init();
};


Φ *Φg = nullptr;

void Φstep(void*);


struct Φ : public Φb
{
  F<bool(Φ&)> go_f;
  uN          nts  = 0;  // number of tracked things

  Φ(Φ&)  = delete;
  Φ(Φ&&) = delete;
  Φ () { A(!Φg, "cannot define multiple Φs in wasm"); Φg = this; }
  ~Φ() { Φg = nullptr; }

  constexpr bool is_async() { return true; }


  operator bool() const { return nts || hn() != forever(); }

  Φ &wake()
    { while (now() >= hn()) l.r(h.top().l), h.pop();
      return *this; }

  Φ &schedule()
    { let t = now();
      if (hn() < forever())
        if (t < hn())
        { let dt = (hn() - t) / 1ms;
          emscripten_async_call(Φstep, this, std::min(dt, Sc<decltype(dt)>(Nl<int>::max()))); }
        else
          emscripten_async_call(Φstep, this, 0);
      return *this; }

  Φ &operator()()
    { wake();
      l.go();
      return schedule(); }


  Φ &go(F<bool(Φ&)> const& = [](Φ&) { return true; })
    { A(0, "Φ is async"); return *this; }

  Φ &go_async(F<bool(Φ&)> &&f = [](Φ &f) { return Sc<bool>(f); })
    { go_f = std::move(f);
      l.go();
      emscripten_async_call(Φstep, this, 0);
      return *this; }
};


void Φstep(void *f_)  // invoked by callbacks to advance Φ
{
  Φ &f = *Rc<Φ*>(f_);
  if (f.go_f(f)) f();
}


}

#include "end.hh"


#endif
