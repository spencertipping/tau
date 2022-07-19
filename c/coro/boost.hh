#ifndef tau_coro_native_h
#define tau_coro_native_h

#include <cassert>

#include "class.hh"

#include "../begin.hh"


namespace tau::coro
{

namespace bc = boost::context;


static bool is_main;


template<class T>
coro<T>::coro(std::string name_, std::function<T()> f)
  : name(name_),
    ret(nullptr),
    k(new bc::continuation)
{
  coro_init();
  *k = bc::callcc(
    [&, f](bc::continuation &&cc) {
      is_main = true;
      *main_coro_state = cc.resume();
      is_main = false;
      *this << f();
      is_main = true;
      return std::move(*main_coro_state);
    });
}


template<class T>
coro<T>::~coro()
{
  if (k)   delete k;
  if (ret) delete ret;
}


template<class T>
coro<T> &coro<T>::operator<<(T &&ret_)
{
  assert(!done());
  assert(!is_main);
  ret  = new T;
  *ret = ret_;

  if (k) { delete k; k = nullptr; }
  return *this;
}


template<class T>
coro<T> &coro<T>::operator()()
{
  assert(!done());
  assert(is_main);
  auto cc = k->resume();
  if (k) *k = std::move(cc);  // NOTE: k can be nulled within k->resume()
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
