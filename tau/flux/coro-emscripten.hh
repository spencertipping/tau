#ifndef tau_flux_coro_emscripten_h
#define tau_flux_coro_emscripten_h


#include <cassert>
#include <cstddef>
#include <functional>
#include <memory>

#include "init.hh"
#include "coro-class.hh"


#ifdef __EMSCRIPTEN__  // reduces clang-lsp errors during development
#include "../module/begin.hh"

namespace tau::flux::coro
{


static coro_k *this_coro_state;
static bool    free_stacks;


template<class T>
void coro_invoke(void *c_)
{
  auto c(*reinterpret_cast<coro<T>**>(c_));
  *c << c->f();
  free_stacks = true;
  yield();
}


template<class T> coro<T>::coro() : thisptr(nullptr) {}
template<class T>
coro<T>::coro(std::function<T()> f_)
  : f      (f_),
    is_done(false)
{
  coro_init();
  k.async_stack = std::malloc(stack_size);
  k.c_stack     = std::malloc(stack_size);
  thisptr       = reinterpret_cast<coro<T>**>(std::malloc(sizeof(coro<T>**)));
  *thisptr      = this;
  emscripten_fiber_init(&k.k, &coro_invoke<T>, thisptr,
                        k.c_stack, stack_size,
                        k.async_stack, stack_size);
}


template<class T>
coro<T>::~coro()
{
  if (k.async_stack) std::free(k.async_stack);
  if (k.c_stack)     std::free(k.c_stack);
  if (thisptr)       std::free(thisptr);
}


template<class T>
coro<T> &coro<T>::operator=(coro<T> &&x)
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
coro<T> &coro<T>::operator()()
{
  assert(!done());
  assert(this_coro_state == &main_coro_state);
  free_stacks     = false;
  let last        = this_coro_state;
  this_coro_state = &k;
  emscripten_fiber_swap(&last->k, &k.k);
  return *this;
}


template<class T>
coro<T> &coro<T>::operator<<(T &&ret_)
{
  is_done = true;
  ret     = std::move(ret_);
  return *this;
}


static void yield()
{
  assert(this_coro_state != &main_coro_state);
  let last = this_coro_state;
  this_coro_state = &main_coro_state;
  emscripten_fiber_swap(&last->k, &this_coro_state->k);

  if (free_stacks)
  {
    let s = this_coro_state;
    if (s->async_stack) { std::free(s->async_stack); s->async_stack = nullptr; }
    if (s->c_stack)     { std::free(s->c_stack);     s->c_stack     = nullptr; }
  }
}


static void coro_init_()
{
  main_coro_state.async_stack = std::malloc(stack_size);
  main_coro_state.c_stack     = nullptr;
  emscripten_fiber_init_from_current_context(&main_coro_state.k,
                                             main_coro_state.async_stack,
                                             stack_size);
  this_coro_state = &main_coro_state;
}


}

#include "../module/end.hh"
#endif


#endif
