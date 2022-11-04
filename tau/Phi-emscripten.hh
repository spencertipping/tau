#ifndef τΦ_emscripten_h
#define τΦ_emscripten_h


#include "arch.hh"

#if τplatform == τplatform_wasm
# include <emscripten.h>
#else
# define EM_ASM(x)
# define EM_JS(...)
#endif


#include "types.hh"
#include "phi.hh"
#include "Lambda.hh"
#include "utf9.hh"

#include "Phi-common.hh"


#include "begin.hh"

namespace τ
{


struct Φ : public Φb
{
  // TODO: track active websockets and other async resources, even though
  // we don't do much with their callbacks
  //
  // TODO: what happens if a websocket onmessage callback happens against
  // a full ζ? We can't block the sender.

  Φ() {}


  Φ &operator()()
    { let t = now();
      // TODO: setTimeout, but asyncified

    }

  operator bool() const
    { return // TODO: return true if ∃ active connections
        hn() != forever(); }

  Φ &go(F<bool(Φ&)> const &f = [](Φ &f) { return Sc<bool>(f); })
    { l.go();
      while (f(*this)) (*this)(), l.go();
      return *this; }
};


}

#include "end.hh"


#endif
