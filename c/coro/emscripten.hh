#ifndef tau_coro_emscripten_h
#define tau_coro_emscripten_h

#include <cassert>
#include <cstddef>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

#include <emscripten/fiber.h>


#include "../begin.hh"


namespace tau::coro
{


struct coro;
static coro *main_coro = nullptr;
static coro *this_coro = nullptr;


void coro_invoke(void *f) { (*reinterpret_cast<std::function<void()>*>(f))(); }


struct coro
{
  std::string            name;
  emscripten_fiber_t     k;
  std::function<void()>  f;
  void                  *async_stack;
  void                  *c_stack;
  bool                   done;


  template<class fn>
  coro(std::string name_, fn f_, std::size_t stack_size = 1048576)
    : name       (name_),
      f          (f_),
      async_stack(std::malloc(stack_size)),
      c_stack    (std::malloc(stack_size)),
      done       (false)
    { emscripten_fiber_init(&k, &coro_invoke, &f,
                            c_stack, stack_size, async_stack, stack_size); }

  coro(std::size_t stack_size = 1048576)
    : name       ("main"),
      async_stack(std::malloc(stack_size)),
      c_stack    (nullptr),
      done       (false)
    { this_coro = this;
      emscripten_fiber_init_from_current_context(&k, async_stack, stack_size); }

  ~coro()
    { if (async_stack) std::free(async_stack);
      if (c_stack)     std::free(c_stack);
      async_stack = c_stack = nullptr; }


  coro &operator()()
    { assert(!done);
      let last_coro = this_coro;
      this_coro     = this;
      emscripten_fiber_swap(&last_coro->k, &k);
      return *this; }


  void finish() { done = true; }


  static coro &main()    { return *main_coro; }
  static coro &current() { return *this_coro; }
  static void  fin()     { this_coro->finish(); (*main_coro)(); }
};


inline void coro_init(std::size_t stack_size = 1048576)
{ main_coro = new coro(stack_size); }


}


#include "../end.hh"

#endif
