#ifndef tau_flux_coro_boost_h
#define tau_flux_coro_boost_h


#include <cassert>

#include "init.hh"
#include "coro-class.hh"


#include "../module/begin.hh"

namespace tau::flux::coro
{

namespace bc = boost::context;


static bool is_main;


template<class T> coro<T>::coro() : thisptr(nullptr) {}
template<class T>
coro<T>::coro(std::function<T()> f)
  : k      (new bc::continuation),
    is_done(false)
{
  coro_init();
  thisptr  = reinterpret_cast<coro<T>**>(std::malloc(sizeof(coro<T>**)));
  *thisptr = this;

  let t = thisptr;
  *k = bc::callcc(
    [t, f](bc::continuation &&cc) {
      is_main = true;
      *main_coro_state = cc.resume();
      **t << f();
      return std::move(*main_coro_state);
    });
}


template<class T>
coro<T>::~coro()
{
  if (k)       delete k;
  if (thisptr) std::free(thisptr);
}


template<class T>
coro<T> &coro<T>::operator=(coro<T> &&c)
{
  thisptr  = c.thisptr;
  *thisptr = this;
  k        = c.k;
  is_done  = c.is_done;
  ret      = std::move(c.ret);

  c.k       = nullptr;
  c.thisptr = nullptr;
  return *this;
}


template<class T>
coro<T> &coro<T>::operator<<(T &&ret_)
{
  assert(!done());
  assert(!is_main);
  is_done = true;
  ret     = std::move(ret_);

  if (k) { delete k; k = nullptr; }
  return *this;
}


template<class T>
coro<T> &coro<T>::operator()()
{
  assert(!done());
  assert(is_main);

  is_main = false;
  auto cc = k->resume();
  if (k) *k = std::move(cc);  // NOTE: k can be nulled within k->resume()
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

#include "../module/end.hh"


#endif
