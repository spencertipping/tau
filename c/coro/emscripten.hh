#ifndef tau_coro_emscripten_h
#define tau_coro_emscripten_h


#include <cassert>
#include <cstddef>
#include <functional>
#include <memory>

#include "class.hh"


#include "../begin.hh"


namespace tau::coro
{


static coro_k *this_coro_state;
static bool    free_stacks;


template<class T, class M>
void coro_invoke(void *c_)
{
  auto &c(*reinterpret_cast<coro<T, M>*>(c_));
  c << c.f();
  free_stacks = true;
  yield();
}


template<class T, class M>
coro<T, M>::coro(std::string         name_,
                 M                  &monitor_,
                 std::function<T()>  f_)
  : name   (name_),
    monitor(monitor_),
    f      (f_),
    ret    (nullptr)
{
  coro_init();
  monitor.init(*this);
  k.async_stack = std::malloc(stack_size);
  k.c_stack     = std::malloc(stack_size);
  emscripten_fiber_init(&k.k, &coro_invoke<T, M>, this,
                        k.c_stack, stack_size,
                        k.async_stack, stack_size);
}


template<class T, class M>
coro<T, M>::~coro()
{
  monitor.finalize(*this);
  if (k.async_stack) std::free(k.async_stack);
  if (k.c_stack)     std::free(k.c_stack);
  if (ret)           delete ret;
}


template<class T, class M>
coro<T, M> &coro<T, M>::operator()()
{
  assert(!done());
  assert(this_coro_state == &main_coro_state);
  free_stacks     = false;
  let last        = this_coro_state;
  this_coro_state = &k;
  monitor.enter(*this);
  emscripten_fiber_swap(&last->k, &k.k);
  monitor.exit(*this);
  return *this;
}


template<class T, class M>
coro<T, M> &coro<T, M>::operator<<(T &&ret_)
{
  ret  = new T;
  *ret = ret_;
  monitor.ret(*this, *ret);
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


#include "../end.hh"

#endif
