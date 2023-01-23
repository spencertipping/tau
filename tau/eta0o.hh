#ifndef τη0o_h
#define τη0o_h


#include <string>

#include <zstd.h>

#include "../dep/picosha3.h"


#include "types.hh"
#include "numerics.hh"
#include "eta-types.hh"
#include "eta0i.hh"

#include "begin.hh"

namespace τ
{


// η₀ structure output stream: incrementally builds an η₀ frame
struct η0o
{
  η0o() : c(0), h(false), sa(false), fv(false), t(η0t::uint_be), d({{0}}) {}
  η0o(η0o &&x) { *this = std::move(x); }

  template<η0at T>
  η0o(T x) : c(0), h(false), sa(false), fv(false), t(η0at_<T>::t) { *this = x; }

  ~η0o() { del_s(); }


  η0o &operator=(η0o &&x) { t = x.t; c = x.c; h = x.h; sa = x.sa; d = x.d; x.sa = false; return *this; }

  η0o &operator=(u64 x)  { del_s(); t = η0t::uint_be;  d.p.u = x; return *this; }
  η0o &operator=(i64 x)  { del_s(); t = η0t::int_be;   d.p.i = x; return *this; }
  η0o &operator=(f64 x)  { del_s(); t = η0t::float_be; d.p.f = x; return *this; }
  η0o &operator=(bool x) { del_s(); t = η0t::boolean;  d.p.b = x; return *this; }


protected:
  u16  c  : 5;  // compression level; 0 = no compression
  u16  h  : 1;  // if true, add a hash
  u16  sa : 1;  // if true, *s_ is valid
  u16  fv : 1;  // if true, ft_ is valid

  η0ft ft_;     // frame type
  η0t  t;       // intended type

  union { η0p p; B *s_; } d;


  bool  has_s() const { return sa; }
  void  del_s()       { if ( has_s()) delete d.s_,  sa = false; }
  B    &s    ()       { if (!has_s()) d.s_ = new B, sa = true; return *d.s_; }
};


static_assert(sizeof(η0o) <= 2 * sizeof(u64));


}


#include "end.hh"


#endif
