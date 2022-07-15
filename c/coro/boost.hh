#ifndef tau_coro_native_h
#define tau_coro_native_h


#include <cassert>
#include <cstddef>

#include <boost/context/continuation.hpp>
#include <boost/context/continuation_fcontext.hpp>


#include "../begin.hh"


namespace tau::coro
{


struct coro;
static coro *main_coro;
static coro *this_coro;


struct coro
{
  // TODO: support custom stack sizing
  boost::context::continuation k;
  bool                         done;

  static coro main_coro_instance;

  coro(void(*f)(), std::size_t stack_size = 1048576)
    : k(boost::context::callcc(
          [&](boost::context::continuation &&cc) {
            main_coro->k = cc.resume();
            (*f)();
            return std::move(main_coro->k);
          })),
      done(false)
    {}

  coro &operator()()
    { assert(!done);
      this_coro = this;
      k = k.resume();
      return *this; }

  static coro &main()    { return *main_coro; }
  static coro &current() { return *this_coro; }
  static void fin()      { this_coro->done = true; (*main_coro)(); }
};


inline void coro_init(std::size_t stack_size = 1048576)
{ main_coro = &coro::main_coro_instance; }


}


#include "../end.hh"

#endif
