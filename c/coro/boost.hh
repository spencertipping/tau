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
  boost::context::continuation *k;

  coro(std::size_t stack_size = 1048576)
    : k(new boost::context::continuation)
    {}

  coro(void(*f)(), std::size_t stack_size = 1048576)
    : k(new boost::context::continuation)
    { *k = boost::context::callcc(
        [&](boost::context::continuation &&cc) {
          // TODO: should we support coros creating other coros?
          *main_coro->k = cc.resume();
          (*f)();
          return std::move(*main_coro->k);
        }); }


  coro &operator()()
    { assert(!done());
      this_coro = this;
      *k = k->resume();
      return *this; }


  bool done() const { return k != nullptr; }
  void finish()
    { if (k) delete k;
      k = nullptr; }


  static coro &main()    { return *main_coro; }
  static coro &current() { return *this_coro; }
  static void fin()      { this_coro->finish(); (*main_coro)(); }
};


inline void coro_init(std::size_t stack_size = 1048576)
{ main_coro = new coro(stack_size); }


}


#include "../end.hh"

#endif
