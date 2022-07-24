#ifndef coro_class_h
#define coro_class_h

#include <functional>
#include <string>

#include "../begin.hh"


#ifdef __EMSCRIPTEN__
#  include <emscripten/fiber.h>
#else
#  include <boost/context/continuation.hpp>
#endif


namespace tau::coro
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


template<class T, class M> struct coro;

struct null_monitor
{
  template<class T> void init    (coro<T, null_monitor> const &)            {}
  template<class T> void enter   (coro<T, null_monitor> const &)            {}
  template<class T> void exit    (coro<T, null_monitor> const &)            {}
  template<class T> void ret     (coro<T, null_monitor> const &, T const &) {}
  template<class T> void finalize(coro<T, null_monitor> const &)            {}
};


template<class T, class M = null_monitor>
struct coro
{
  std::string        name;
  std::function<T()> f;
  coro_k             k;
  T                 *ret;
  M                 &monitor;

  coro(std::string, M &, std::function<T()>);
  ~coro();

  bool  done()   const { return  ret != nullptr; }
  T    &result() const { return *ret; };
  coro &operator()();
  coro &operator<<(T&&);
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


#include "../end.hh"

#endif
