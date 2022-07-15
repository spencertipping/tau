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
static coro *main_coro = nullptr;
static coro *this_coro = nullptr;


struct coro
{
  // TODO: support custom stack sizing
  std::string                   name;
  boost::context::continuation *k;
  bool                          done;

  coro(std::size_t stack_size = 1048576)
    : name("main"),
      k(new boost::context::continuation),
      done(false)
    {}

  template<class fn>
  coro(std::string name_, fn f, std::size_t stack_size = 1048576)
    : name(name_),
      k(new boost::context::continuation),
      done(false)
    { *k = boost::context::callcc(
        [&, f](boost::context::continuation &&cc) {
          // TODO: should we support coros creating other coros?
          *main_coro->k = cc.resume();
          f();
          return std::move(*main_coro->k);
        }); }

  ~coro() { delete k; }


  coro &operator()()
    { assert(!done);
      this_coro = this;
      *k = k->resume();
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
