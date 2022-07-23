#ifndef tau_coro_native_h
#define tau_coro_native_h

#include <cassert>

#include "class.hh"

#include "../begin.hh"


namespace tau::coro
{

namespace bc = boost::context;


static bool is_main;


template<class T, class M>
coro<T, M>::coro(std::string         name_,
                 M                  &monitor_,
                 std::function<T()>  f)
  : name   (name_),
    monitor(monitor_),
    ret    (nullptr),
    k      (new bc::continuation)
{
  coro_init();
  monitor.init(*this);
  *k = bc::callcc(
    [&, f](bc::continuation &&cc) {
      is_main = true;
      *main_coro_state = cc.resume();
      *this << f();
      return std::move(*main_coro_state);
    });
}


template<class T, class M>
coro<T, M>::~coro()
{
  monitor.finalize(*this);
  if (k)   delete k;
  if (ret) delete ret;
}


template<class T, class M>
coro<T, M> &coro<T, M>::operator<<(T &&ret_)
{
  assert(!done());
  assert(!is_main);
  ret  = new T;
  *ret = ret_;
  monitor.ret(*this, *ret);

  if (k) { delete k; k = nullptr; }
  return *this;
}


template<class T, class M>
coro<T, M> &coro<T, M>::operator()()
{
  assert(!done());
  assert(is_main);

  is_main = false;
  monitor.enter(*this);
  auto cc = k->resume();
  if (k) *k = std::move(cc);  // NOTE: k can be nulled within k->resume()
  monitor.exit(*this);
  is_main = true;
  return *this;
}


static void yield()
{
  assert(!is_main);
  *main_coro_state = main_coro_state->resume();
}


static void coro_init_()
{
  main_coro_state = new bc::continuation;
}


}


#include "../end.hh"

#endif
