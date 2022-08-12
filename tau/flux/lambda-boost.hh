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
λ<T>::λ(F<T()> &&f_)
  : f      (std::move(f_)),
    k      (nullptr),
    is_done(false)
{
  λinit();
  thisptr  = Rc<λ<T>**>(std::malloc(sizeof(λ<T>**)));
  *thisptr = this;

  if (λmi)
    k = new λbc::continuation(λbc::callcc(
      [t = thisptr](λbc::continuation &&cc) {
        *λmk = cc.resume();
        **t << (*t)->f();
        return std::move(*λmk);
      }));
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
  f        = std::move(c.f);
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
  if (!k)
  {
    // TODO: this can probably be optimized? (minor, just the
    // extra cc.resume)
    k = new λbc::continuation;
    auto cc = λbc::callcc(
      [t = thisptr](λbc::continuation &&cc) {
        *λmk = cc.resume();
        **t << (*t)->f();
        return std::move(*λmk);
      });
    cc = cc.resume();
    if (k) *k = std::move(cc);
  }
  else
  {
    auto cc = k->resume();
    if (k) *k = std::move(cc);  // NOTE: k can be nulled within k->resume()
  }

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
