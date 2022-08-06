#ifndef tau_flux_Λ_h
#define tau_flux_Λ_h


#include "../types.hh"
#include "init.hh"
#include "lambda.hh"


#include "../module/begin.hh"

namespace tau::flux
{


enum λs  // lambda runnability state (informative, not canonical)
{
  λR,    // runnable
  λS,    // stopped
  λI,    // blocked on read from ζ
  λO,    // blocked on write to ζ
  λΘ,    // waiting for a time
  λZ,    // done (zombie)
};


typedef uN      λi;  // λ identifier
typedef int     Λr;  // type of value returned to Λ
typedef F<Λr()> λf;


struct Λ             // manager of λs
{
  struct Λλ { λ<Λr> l; λs s; };

  M<λi, Λλ> ls;      // all λs
  Q<λi>     rq;      // run schedule
  λi        ni{1};   // next λi

  λi   c(λf f)       { let   i = ni++;     ls.emplace(i, Λλ{λ<Λr>(f), λS}); r(i);   return i; }
  bool e(λi i) const {                                                              return ls.contains(i); }
  uN   n()     const {                                                              return ls.size(); }
  Λ   &r(λi i)       { auto &l = ls.at(i); assert(l.s != λR); rq.push(i); l.s = λR; return *this; }
  Λ   &x(λi i)       {                     assert(e(i));      ls.erase(i);          return *this; }


};


}

#include "../module/end.hh"


#endif
