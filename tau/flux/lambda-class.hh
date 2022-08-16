#ifndef tau_flux_λ_class_h
#define tau_flux_λ_class_h


#include "../arch.hh"
#include "../debug.hh"


#if tau_arch == tau_arch_wasm64 || tau_arch == tau_arch_wasm32
# include <emscripten/fiber.h>
#elif tau_assume_emscripten
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


#include "../types.hh"
#include "types.hh"

#include "../module/begin.hh"

namespace tau::flux
{


#if tau_arch == tau_arch_wasm64                   \
    || tau_arch == tau_arch_wasm32                \
    || tau_assume_emscripten
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


void λy();                       // yield
void λinit_();                   // implementation-specific main λ init
uN const constexpr λss = 65536;  // stack size
λk                 λmk;          // main continuation


// NOTE: init mechanics used only by λ ctor; not useful to importers
// of λ.hh, as they are automatically managed.
void λinit()
{
  static bool called = false;
  if (called) return;
  λinit_();
  called = true;
}


template<class T>
struct λ
{
  F<T()> f;
  λk     k;
  bool   is_done;
  T      ret;
  λ<T> **thisptr;

  λ();
  λ(F<T()>&&);
  ~λ();

  bool     done()   const {                  return is_done; }
  T const &result() const { assert(is_done); return ret; };

  λ<T> &operator=(λ<T> &&);
  λ    &operator()();
  λ    &operator<<(T&&);
  λ    &operator<<(T const &);
};


}

#include "../module/end.hh"


#endif
