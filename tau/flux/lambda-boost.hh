#ifndef tau_flux_λ_boost_h
#define tau_flux_λ_boost_h


#include "../types.hh"
#include "lambda-class.hh"


#include "../module/begin.hh"

namespace tau::flux
{

namespace λbc = boost::context;


bool λmi;  // true if current continuation is main


template<class T> λ<T>::λ() : k(nullptr), thisptr(nullptr) {}
template<class T>
λ<T>::λ(F<T()> &&f)
  : k      (new λbc::continuation),
    is_done(false)
{
  λinit();
  thisptr  = Rc<λ<T>**>(std::malloc(sizeof(λ<T>**)));
  *thisptr = this;

  let t = thisptr;
  std::cout << "k = " << k << std::endl;
  std::cout << "thisptr = " << thisptr << std::endl;
  *k = λbc::callcc(
    [t, f = std::move(f)](λbc::continuation &&cc) {
      std::cout << "inside new continuation" << std::endl;
      let m = λmi;
      if (m) *λmk = cc.resume();
      else   cc   = cc.resume();
      **t << f();
      return m ? std::move(*λmk) : std::move(cc);
    });
}


template<class T>
λ<T>::~λ()
{
  if (k)       delete k;
  if (thisptr) std::free(thisptr);
}


template<class T>
λ<T> &λ<T>::operator=(λ<T> &&c)
{
  if (k) delete k;
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
λ<T> &λ<T>::operator<<(T &&ret_)
{
  assert(!done());
  assert(!λmi);
  is_done = true;
  ret     = std::move(ret_);

  if (k) { delete k; k = nullptr; }
  return *this;
}


template<class T>
λ<T> &λ<T>::operator()()
{
  assert(!done());
  assert(λmi);

  λmi = false;
  auto cc = k->resume();
  if (k) *k = std::move(cc);  // NOTE: k can be nulled within k->resume()
  λmi = true;
  return *this;
}


void λy()
{
  assert(!λmi);
  *λmk = λmk->resume();
}


void λinit_()
{
  λmi = true;
  λmk = new λbc::continuation;
}


}

#include "../module/end.hh"


#endif
