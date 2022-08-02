#ifndef tau_flux_coro_class_h
#define tau_flux_coro_class_h

#include <functional>
#include <string>

#include "../module/begin.hh"


#ifdef __EMSCRIPTEN__
#  include <emscripten/fiber.h>
#else
#  include <boost/context/continuation.hpp>
#endif


namespace tau::flux::coro
{


#ifdef __EMSCRIPTEN__
  static constexpr size_t stack_size = 1048576;
  struct coro_k
  {
    emscripten_fiber_t  k;
    void               *async_stack;
    void               *c_stack;
  };
#else
  namespace bc = boost::context;
  typedef bc::continuation *coro_k;
#endif


static coro_k main_coro_state;


template<class T>
struct coro
{
  std::function<T()> f;
  coro_k             k;
  bool               is_done;
  T                  ret;
  coro<T>          **thisptr;

  coro();
  coro(std::function<T()>);
  ~coro();

  bool     done()   const { return is_done; }
  T const &result() const { return ret; };

  coro<T> &operator=(coro<T> &&);
  coro    &operator()();
  coro    &operator<<(T&&);
  coro    &operator<<(T const &);
};


static void yield();


// NOTE: init mechanics used only by coro ctor; not useful to importers
// of coro.hh, as they are automatically managed.
static void coro_init_();
static void coro_init()
{
  static bool called = false;
  if (called) return;
  coro_init_();
  called = true;
}


}


#include "../module/end.hh"

#endif
