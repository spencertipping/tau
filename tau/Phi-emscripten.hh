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
# include <cstddef>
  struct emscripten_fiber_t {};
  void emscripten_fiber_swap(emscripten_fiber_t*, emscripten_fiber_t*);
  void emscripten_fiber_init_from_current_context(emscripten_fiber_t*, void*, size_t);
  void emscripten_fiber_init(emscripten_fiber_t*,
                             void(*)(void*),
                             void*,
                             void*, size_t, void*, size_t);

  void emscripten_async_call(void(*)(void*), void*, int);
#endif


#include "types.hh"
#include "phi.hh"
#include "Lambda.hh"
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

void Φgo(void*);


struct Φ : public Φb
{
  emscripten_fiber_t *main_thread = nullptr;
  emscripten_fiber_t *go_thread   = nullptr;
  F<bool(Φ&)>  const *go_f        = nullptr;
  uN                  nts         = 0;  // number of tracked things

  Φ(Φ &) = delete;
  Φ(Φ&&) = delete;
  Φ () { A(!Φg, "cannot define multiple Φs in wasm"); Φg = this; }
  ~Φ() { Φg = nullptr; }


  Φ &operator()()
    { let t = now();
      std::cout << "Φ() at " << t - t0 << std::endl;
      if (t < hn() && hn() < forever())
      { let dt = (hn() - t) / 1ms;
        emscripten_async_call(Φgo, this, std::min(dt, Sc<decltype(dt)>(Nl<int>::max()))); }
      while (now() >= hn()) l.r(h.top().l), h.pop();
      return *this; }

  operator bool() const { return nts || hn() != forever(); }

  Φ &go(F<bool(Φ&)> const &f = [](Φ &f) { return Sc<bool>(f); })
    { void *as1 = malloc(λss);
      void *cs1 = malloc(λss);
      void *as2 = malloc(λss);

      std::cout << "Φ.go" << std::endl;

      go_f = &f;
      emscripten_fiber_init_from_current_context(main_thread, as2, λss);
      emscripten_fiber_init(go_thread, Φgo, this, cs1, λss, as1, λss);

      l.go();

      std::cout << "Φ.go pre-swap" << std::endl;
      emscripten_fiber_swap(main_thread, go_thread);
      std::cout << "Φ.go post-swap" << std::endl;
      go_thread   = nullptr;
      main_thread = nullptr;
      go_f        = nullptr;

      free(as1);
      free(cs1);
      free(as2);
      return *this; }

  void go_ret() { emscripten_fiber_swap(go_thread, main_thread); }
};


void Φgo(void *f_)  // invoked by callbacks to advance Φ
{
  Φ &f = *Rc<Φ*>(f_);
  if (!(*f.go_f)(f))
  {
    std::cout << "go_ret" << std::endl;
    f.go_ret();
  }
  else
  {
    f();
    f.l.go();
  }
}


}

#include "end.hh"


#endif
