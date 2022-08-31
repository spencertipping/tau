#ifndef tau_λ_emscripten_h
#define tau_λ_emscripten_h


#include <memory>


#include "types.hh"


// For interactive development with clang/lsp; this is a nop in production
#define tau_assume_emscripten 1
# include "lambda-class.hh"
#undef tau_assume_emscripten


#include "begin.hh"

namespace tau
{


λk   *λthis;
bool  λsx;    // if true, free stacks


template<class T>
void λinvoke(void *λ_)
{
  auto f(*Rc<λ<T>**>(λ_));
  *f << f->f();
  λsx = true;
  λy();
}


template<class T> λ<T>::λ() : thisptr(nullptr) {}
template<class T>
λ<T>::λ(F<T()> &&f_)
  : f      (std::move(f_)),
    is_done(false)
{
  λinit();
  k.async_stack = std::malloc(λss);
  k.c_stack     = std::malloc(λss);
  thisptr       = Rc<λ<T>**>(std::malloc(sizeof(λ<T>**)));
  *thisptr      = this;
  emscripten_fiber_init(&k.k, &λinvoke<T>, thisptr,
                        k.c_stack,     λss,
                        k.async_stack, λss);
}


template<class T>
λ<T>::~λ()
{
  if (k.async_stack) std::free(k.async_stack);
  if (k.c_stack)     std::free(k.c_stack);
  if (thisptr)       std::free(thisptr);
}


template<class T>
λ<T> &λ<T>::operator=(λ<T> &&x)
{
  f             = std::move(x.f);
  k.k           = std::move(x.k.k);
  k.async_stack = x.k.async_stack;
  k.c_stack     = x.k.c_stack;
  is_done       = x.is_done;
  ret           = std::move(x.ret);
  thisptr       = x.thisptr;
  *thisptr      = this;

  x.thisptr       = nullptr;
  x.k.async_stack = nullptr;
  x.k.c_stack     = nullptr;

  return *this;
}


template<class T>
λ<T> &λ<T>::operator()()
{
  assert(!done());
  assert(λthis == &λmk);
  λsx   = false;
  let t = λthis;
  λthis = &k;
  emscripten_fiber_swap(&t->k, &k.k);
  return *this;
}


template<class T>
λ<T> &λ<T>::operator<<(T &&r_)
{
  is_done = true;
  ret     = std::move(r_);
  return *this;
}


void λy()
{
  assert(λthis != &λmk);
  let t = λthis;
  λthis = &λmk;
  emscripten_fiber_swap(&t->k, &λthis->k);

  if (λsx)
  {
    // FIXME: why does λthis contain disposable stacks if we assigned
    // it from &λmk above?
    //let s = λthis;
    let s = t;
    if (s->async_stack) { std::free(s->async_stack); s->async_stack = nullptr; }
    if (s->c_stack)     { std::free(s->c_stack);     s->c_stack     = nullptr; }
  }
}


void λinit_()
{
  λmk.async_stack = std::malloc(λss);
  λmk.c_stack     = nullptr;
  emscripten_fiber_init_from_current_context(&λmk.k,
                                             λmk.async_stack,
                                             λss);
  λthis = &λmk;
}


}

#include "end.hh"


#endif
