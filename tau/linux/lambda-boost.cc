#include "../types.hh"
#include "../lambda-types.hh"
#include "../lambda-class.hh"


#include "../begin.hh"

namespace τ
{

namespace λbc = boost::context;


bool λmi;  // true if current continuation is main


void λm(λbc::continuation &&cc)
{
  if (cc) **λmk() = mo(cc);
}


λ::λ(λf &&f_)
  : f      (mo(f_)),
    k      (nullptr),
    is_done(false)
{
  λinit();
  if (λmi)
    k = new λbc::continuation(λbc::callcc(
      std::allocator_arg, λbc::fixedsize_stack(λss),
      [&](λbc::continuation &&cc)
        { λm(cc.resume());
          f();
          fin();
          return mo(**λmk()); }));
}


λ::~λ()
{
  if (k) delete k;
}


void λ::fin()
{
  A(!done(), "fin() called on done λ");
  A(!λmi,    "fin() called on main λ");
  is_done = true;
  if (k) { delete k; k = nullptr; }
}


λ &λ::operator()()
{
  A(!done(), "λ called after fin()");
  A(λmi,     "λ called not from main");

  λmi = false;
  if (!k)
  {
    // NOTE: I believe we need to capture λmk from the inner continuation's
    // frame of reference so it knows where to return. I haven't fully worked
    // this out yet, but cc.resume() at the beginning makes it work reliably.
    k = new λbc::continuation;
    auto cc = λbc::callcc(
      std::allocator_arg, λbc::fixedsize_stack(λss),
      [&](λbc::continuation &&cc)
        { λm(cc.resume());
          f();
          fin();
          return mo(**λmk()); });
    cc = cc.resume();
    if (k) *k = mo(cc);
  }
  else
  {
    auto cc = k->resume();
    if (k) *k = mo(cc);  // NOTE: k can be nulled within k->resume()
  }

  λmi = true;
  return *this;
}


void λy()
{
  A(!λmi,   "λy() from main");
  A(*λmk(), "λy() with no main continuation");
  **λmk() = (*λmk())->resume();
}


void λinit_()
{
  λmi = true;
  *λmk() = new λbc::continuation;
}


}

#include "../end.hh"
