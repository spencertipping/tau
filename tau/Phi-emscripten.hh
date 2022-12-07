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


  Φ &operator()()
    { let t = now();
      std::cout << "Φ() at " << t - t0 << std::endl;
      if (t < hn() && hn() < forever())
      { let dt = (hn() - t) / 1ms;
        std::cout << "scheduling async in " << dt << " ms" << std::endl;
        emscripten_async_call(Φstep, this, std::min(dt, Sc<decltype(dt)>(Nl<int>::max()))); }
      while (now() >= hn()) l.r(h.top().l), h.pop();
      return *this; }

  operator bool() const { return nts || hn() != forever(); }

  Φ &go(F<bool(Φ&)> const& = [](Φ&) { return true; })
    { A(0, "Φ is async"); return *this; }

  Φ &go_async(F<bool(Φ&)> &&f = [](Φ &f) { return Sc<bool>(f); })
    { go_f = std::move(f);
      emscripten_async_call(Φstep, this, 0);
      return *this; }
};


void Φstep(void *f_)  // invoked by callbacks to advance Φ
{
  Φ &f = *Rc<Φ*>(f_);
  std::cout << "Φstep called" << std::endl;
  if (f.go_f(f))
  {
    std::cout << "calling Φ()" << std::endl;
    f();
    f.l.go();
  }
}


}

#include "end.hh"


#endif
