#ifndef τλ_boost_h
#define τλ_boost_h


#include "types.hh"
#include "lambda-class.hh"


#include "begin.hh"

namespace τ
{

namespace λbc = boost::context;


bool λmi;  // true if current continuation is main


void λm(λbc::continuation &&cc)
{
  if (cc) *λmk = std::move(cc);
}


λ::λ() : k(nullptr), thisptr(nullptr) {}
λ::λ(F<void()> &&f_)
  : f      (std::move(f_)),
    k      (nullptr),
    is_done(false)
{
  λinit();
  thisptr  = Rc<λ**>(std::malloc(sizeof(λ**)));
  *thisptr = this;

  if (λmi)
    k = new λbc::continuation(λbc::callcc(
      [t = thisptr](λbc::continuation &&cc) {
        λm(cc.resume());
        (*t)->f();
        (*t)->fin();
        return std::move(*λmk);
      }));
}


λ::~λ()
{
  if (k)       delete k;
  if (thisptr) std::free(thisptr);
}


λ &λ::operator=(λ &&c)
{
  if (k) delete k;
  thisptr  = c.thisptr;
  *thisptr = this;
  k        = c.k;
  f        = std::move(c.f);
  is_done  = c.is_done;

  c.k       = nullptr;
  c.thisptr = nullptr;
  return *this;
}


void λ::fin()
{
  assert(!done());
  assert(!λmi);
  is_done = true;
  if (k) { delete k; k = nullptr; }
}


λ &λ::operator()()
{
  assert(!done());
  assert(λmi);

  λmi = false;
  if (!k)
  {
    // NOTE: I believe we need to capture λmk from the inner continuation's
    // frame of reference so it knows where to return. I haven't fully worked
    // this out yet, but cc.resume() at the beginning makes it work reliably.
    k = new λbc::continuation;
    auto cc = λbc::callcc(
      [t = thisptr](λbc::continuation &&cc) {
        λm(cc.resume());
        (*t)->f();
        (*t)->fin();
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
  assert(*λmk);
  *λmk = λmk->resume();
}


void λinit_()
{
  λmi = true;
  λmk = new λbc::continuation;
}


}

#include "end.hh"


#endif
