#include <cstring>


#include "zeta.hh"
#include "begin.hh"

namespace τ
{


ζ &ζ::resize(uN c_)
{
  if (c_ == c) return *this;
  A(c_ <= ra(),
    "cannot resize ζ to be smaller than its contents" <<
    "(c = " << c << ", c_ = " << c_ << ", ra = " << ra());

  let ys = new u8[c_];
  if (wrapped())
  { memcpy(ys,             xs + wi, ci - wi);  // (wi..ci)
    memcpy(ys + (ci - wi), xs,      ri); }     // (0..ri)
  else
    memcpy(ys, xs + ri, wi - ri);

  let r = ra();
  ri = ci = 0;
  wi = r;

  A(ra() == r, "resize() modified ra(): " << r << " → " << ra());

  delete[] xs;
  xs = ys;
  c  = c_;
  return *this;
}


O &operator<<(O &s, ζ const &z)
{
  return s << "ζ[c=" << z.c << " ri=" << z.ri
           << " wi=" << z.wi << " ci=" << z.ci
           << " ra=" << z.ra() << " wa=" << z.wa() << "]";
}


}

#include "end.hh"
