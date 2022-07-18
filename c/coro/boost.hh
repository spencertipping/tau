#ifndef tau_coro_native_h
#define tau_coro_native_h

#include <cassert>

#include "class.hh"

#include "../begin.hh"


namespace tau::coro
{


template<class T>
coro<T>::coro(std::string name_, std::function<T()> f)
  : name(name_),
    ret(nullptr),
    k(new boost::context::continuation)
{ *k = boost::context::callcc(
    [&, f](boost::context::continuation &&cc) {
      *main_coro_state = cc.resume();
      *this << f();
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
  ret  = new T;
  *ret = ret_;

  if (k) { delete k; k = nullptr; }
  return *this;
}


template<class T>
coro<T> &coro<T>::operator()()
{
  assert(!done());
  auto cc = k->resume();
  if (k) *k = std::move(cc);
  return *this;
}


void yield()
{
  *main_coro_state = main_coro_state->resume();
}


void coro_init()
{
  main_coro_state = new boost::context::continuation;
}


}


#include "../end.hh"

#endif
