#ifndef tau_gate_h
#define tau_gate_h


#include "types.hh"
#include "Lambda.hh"

#include "begin.hh"

namespace tau
{


struct λg
{
  Λ     &l;
  V<λi>  ps;

  λg &y(λs s) { ps.push_back(l.i()); l.y(s); return *this; }
  λg &w()     { for (let p : ps) l.r(p); if (!ps.empty()) ps.erase(ps.begin(), ps.end()); return *this; }
};


}

#include "end.hh"


#endif
