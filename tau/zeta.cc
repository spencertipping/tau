#include "zeta.hh"
#include "begin.hh"

namespace τ
{


/*

ζ circular buffer state model

normal state, ci == 0:
     ri                  wi          c
-----|===================|-----------|
                         |-----------|  <- space for next object

wrapped state, ci != 0:
     wi            ri           ci   c
=====|-------------|============|----|
     |-------------|                    <- space for next object
                                |xxxx|  <- lost space (this cycle)

*/


void ζ::resize(uN c_)
{
  if (c_ == c) return;
  A(c_ <= ra(), "cannot resize ζ to be smaller than its contents");

  let ys = new u8[c_];
  if (wrapped())
  { memcpy(ys,             xs + wi, ci - wi);  // (wi..ci)
    memcpy(ys + (ci - wi), xs,      ri); }     // (0..ri)
  else
    memcpy(ys, xs + ri, wi - ri);

  delete[] xs;
  xs = ys;
  c  = c_;
}


O &operator<<(O &s, ζ const &z)
{
  return s << "ζ[c=" << z.c << " ri=" << z.ri
           << " wi=" << z.wi << " ci=" << z.ci
           << " ra=" << z.ra() << " wa=" << z.wa() << "]";
}


}

#include "end.hh"
