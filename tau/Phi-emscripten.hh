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
#endif


#include "types.hh"
#include "phi.hh"
#include "Lambda.hh"
#include "utf9.hh"

#include "Phi-common.hh"


#include "begin.hh"

namespace τ
{


// TODO: drive this with callbacks; each one should update state and
// then invoke the main Φ event loop, since callbacks all happen on
// the main thread

// .go() can fiber_swap out and then back once all callbacks are done
// (realistically, .go() is free to never return)


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


struct Φ : public Φb
{
  // TODO: track active websockets and other async resources, even though
  // we don't do much with their callbacks
  //
  // TODO: what happens if a websocket onmessage callback happens against
  // a full ζ? We can't block the sender.

  emscripten_fiber_t *main_thread = nullptr;
  emscripten_fiber_t *this_thread = nullptr;

  Φ() {}


  Φ &operator()()
    { let t = now();
      TODO("Φ step");
      // TODO: setTimeout, but asyncified

    }

  operator bool() const
    { return // TODO: return true if ∃ active connections
        hn() != forever(); }

  Φ &go(F<bool(Φ&)> const &f = [](Φ &f) { return Sc<bool>(f); })
    { // TODO: fiberswap

      l.go();
      while (f(*this)) (*this)(), l.go();
      return *this; }
};


}

#include "end.hh"


#endif
