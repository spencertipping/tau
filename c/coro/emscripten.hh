#ifndef tau_coro_emscripten_h
#define tau_coro_emscripten_h

#include <cassert>
#include <cstddef>
#include <memory>
#include <emscripten/fiber.h>

#include "../begin.hh"


namespace tau::coro
{


struct coro;
static coro *main_coro;
static coro *this_coro;


struct coro
{
  emscripten_fiber_t  k;
  void               *async_stack;
  void               *c_stack;
  bool                done;


  coro(void(*f)(), std::size_t stack_size = 1048576)
    : async_stack(std::malloc(stack_size)),
      c_stack    (std::malloc(stack_size)),
      done(false)
    { emscripten_fiber_init(&k, f, nullptr,
                            c_stack, stack_size, async_stack, stack_size); }

  coro(std::size_t stack_size = 1048576)
    : async_stack(std::malloc(stack_size)),
      c_stack    (nullptr),
      done(false)
    { this_coro = this;
      emscripten_fiber_init_from_current_context(&k, async_stack, stack_size); }

  ~coro()
    { if (async_stack) std::free(async_stack);
      if (c_stack)     std::free(c_stack); }

  coro &operator()()
    { assert(!done);
      let last_coro = this_coro;
      this_coro     = this;
      emscripten_fiber_swap(&last_coro->k, &k); }

  static coro &main()    { return *main_coro; }
  static coro &current() { return *this_coro; }
  static void fin()      { this_coro->done = true; (*main_coro)(); }

};


inline void coro_init(std::size_t stack_size = 1048576)
{ main_coro = new coro(stack_size); }


}


#include "../end.hh"

#endif
