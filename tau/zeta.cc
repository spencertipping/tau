#include <cstring>


#include "zeta.hh"
#include "begin.hh"

namespace τ
{


static uN ζmem_ = 0;

void ζtrack_alloc(uN c) { ζmem_ += c; }
void ζtrack_free (uN c) { ζmem_ -= c; }
uN   ζallocated  ()     { return ζmem_; }


ζ &ζ::resize(uN c_)
{
  if (c_ == c) return *this;
  A(c_ >= ra(),
    "cannot resize ζ to be smaller than its contents" <<
    "(c = " << c << ", c_ = " << c_ << ", ra = " << ra());

  ζtrack_alloc(c_);
  let ys = new u8[c_];
  if (wrapped())
  {
    memcpy(ys,             xs + ri, ci - ri);  // (ri..ci)
    memcpy(ys + (ci - ri), xs,      wi);       // (0..wi)
  }
  else
    memcpy(ys, xs + ri, wi - ri);

  let r = ra();
  ri = ci = 0;
  wi = r;

  A(ra() == r, "resize() modified ra(): " << r << " → " << ra());

  delete[] xs;
  ζtrack_free(c);
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
