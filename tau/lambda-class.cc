#include "../tau.hh"
#include "begin.hh"

namespace τ
{


λk λmk_;
λk *λmk() { return &λmk_; }


// NOTE: init mechanics used only by λ ctor; not useful to importers
// of λ.hh, as they are automatically managed.
void λinit()
{
  static bool called = false;
  if (called) return;
  λinit_();
  called = true;
}


}

#include "end.hh"
