#include "lambda-class.hh"
#include "begin.hh"

namespace τ
{


static uN λmem_ = 0;

void λtrack_alloc(uN s) { λmem_ += s; }
void λtrack_free (uN s) { λmem_ -= s; }
uN   λallocated  ()     { return λmem_; }


static λk λmk_;
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
