#include <memory>


#include "../types.hh"
#include "../lambda-types.hh"


// For interactive development with clang/lsp; this is a nop in production
#define τassume_emscripten 1
# include "../lambda-class.hh"
#undef τassume_emscripten


#include "../begin.hh"

namespace τ
{


λk   *λthis;
bool  λsx;    // if true, free stacks


void λinvoke(void *λ_)
{
  auto f(Rc<λ*>(λ_));
  f->f();
  f->fin();
  λsx = true;
  λy();
}


λ::λ(λf &&f_)
  : f      (mo(f_)),
    is_done(false)
{
  λinit();
  λtrack_alloc(λss * 2);
  k.async_stack = ma(λss);
  k.c_stack     = ma(λss);
  emscripten_fiber_init(&k.k, &λinvoke, this,
                        k.c_stack,     λss,
                        k.async_stack, λss);
}


λ::~λ()
{
  if (k.async_stack) fr(k.async_stack), λtrack_free(λss);
  if (k.c_stack)     fr(k.c_stack),     λtrack_free(λss);
}


λ &λ::operator()()
{
  assert(!done());
  assert(λthis == λmk());
  λsx   = false;
  let t = λthis;
  λthis = &k;
  emscripten_fiber_swap(&t->k, &k.k);
  return *this;
}


void λ::fin()
{
  assert(!done());
  assert(λthis != λmk());
  is_done = true;
}


void λy()
{
  assert(λthis != λmk());
  let t = λthis;
  λthis = λmk();
  emscripten_fiber_swap(&t->k, &λthis->k);

  if (λsx)
  {
    // FIXME: why does λthis contain disposable stacks if we assigned
    // it from λmk() above?
    //let s = λthis;
    let s = t;
    if (s->async_stack) { λtrack_free(λss); fr(s->async_stack); s->async_stack = nullptr; }
    if (s->c_stack)     { λtrack_free(λss); fr(s->c_stack);     s->c_stack     = nullptr; }
  }
}


void λinit_()
{
  λmk()->async_stack = ma(λss);
  λmk()->c_stack     = nullptr;
  λtrack_alloc(λss);
  emscripten_fiber_init_from_current_context(&λmk()->k,
                                             λmk()->async_stack,
                                             λss);
  λthis = λmk();
}


}

#include "../end.hh"
