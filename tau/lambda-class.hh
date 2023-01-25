#ifndef τλclass_h
#define τλclass_h


#include "arch.hh"
#include "debug.hh"


#if τplatform == τplatform_wasm
# include <emscripten/fiber.h>
#elif τassume_emscripten
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
#else
# include <boost/context/continuation.hpp>
#endif


#include "types.hh"
#include "lambda-types.hh"


#include "begin.hh"

namespace τ
{


#if τplatform == τplatform_wasm || τassume_emscripten
  struct λk
  {
    emscripten_fiber_t  k;
    void               *async_stack;
    void               *c_stack;
  };
#else
  namespace bc = boost::context;
  typedef bc::continuation *λk;
#endif


void λy();        // yield
void λinit_();    // implementation-specific main λ init
uNc λss = 65536;  // stack size
λk  λmk;          // main continuation


// NOTE: init mechanics used only by λ ctor; not useful to importers
// of λ.hh, as they are automatically managed.
void λinit()
{
  static bool called = false;
  if (called) return;
  λinit_();
  called = true;
}


struct λ
{
  // NOTE: don't modify these outside of this module; it's easier not to
  // protect them, but they should be treated as protected
  F<void()> f;
  λk        k;
  bool      is_done;
  λ       **thisptr;

  λ();
  λ(F<void()>&&);
  ~λ();

  void fin();
  bool done() const { return is_done; }

  λ &operator=(λ&&);
  λ &operator()();
};


}

#include "end.hh"


#endif
