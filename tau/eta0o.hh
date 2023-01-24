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
  η0o() : c_(0), h_(false), f_(false), sa(false), fv(false), t_(η0t::uint_be), d({{0}}), cs(nullptr) {}
  η0o(η0o &&x) { *this = std::move(x); }

  template<η0at T>
  η0o(T x) : c_(0), h_(false), f_(false), sa(false), fv(false), t_(η0at_<T>::t), cs(nullptr) { *this = x; }

  ~η0o() { del_s(); }


  η0o &operator=(η0o &&x)
    { c_  = x.c_;  h_ = x.h_; f_ = x.f_; sa = x.sa; fv = x.fv;
      ft_ = x.ft_; t_ = x.t_; d  = x.d;  cs = x.cs;
      x.cs = nullptr; x.sa = false;  // transfer ownership
      return *this; }

  η0o &operator=(u64  x) { del_s(); t_ = η0t::uint_be;  d.p.u = x; return *this; }
  η0o &operator=(i64  x) { del_s(); t_ = η0t::int_be;   d.p.i = x; return *this; }
  η0o &operator=(f64  x) { del_s(); t_ = η0t::float_be; d.p.f = x; return *this; }
  η0o &operator=(bool x) { del_s(); t_ = η0t::boolean;  d.p.b = x; return *this; }


  // Inner size of the fully-encoded frame data: that is, the size that
  // the frame will end up describing.
  uN isize() const
    { switch (t_)
      {
      case η0t::η0:       return sizeof(d.p.p);
      case η0t::boolean:  return sizeof(d.p.b);
      case η0t::uint_be:  return su(d.p.u);
      case η0t::int_be:   return su(d.p.i);
      case η0t::float_be: return sizeof(d.p.f);
      default:
        return c_ && cs ? 8 + cs->size() : sa ? d.s_->size() : 0;
      } }

  // Outer size of the frame once serialized -- i.e. including the frame
  // header.
  uN osize() const
    { switch (ft())
      {
      case η0ft::s: return 1 + isize();
      case η0ft::m: return 2 + isize();
      case η0ft::l:
      case η0ft::d: return 2 + ubytes(isize()) + (h_ ? 32 : 0) + isize();
      } }


  η0ft ft() const
    { if (!fv)
      { let s = isize();
        let t = Sc<u8>(t_);
        ft_ = h_                                              ? η0ft::d
            : c_ || t >= 64 || isize() >= 128                 ? η0ft::l
            : t >= 32 || s != 1 && s != 2 && s != 4 && s != 8 ? η0ft::m
            :                                                   η0ft::s;
        fv = true; }
      return ft_; }


protected:
  u16         c_ : 5;  // compression level; 0 = no compression
  u16         h_ : 1;  // if true, add a hash
  u16         f_ : 1;  // if true, we're flagged
  u16         sa : 1;  // if true, *s_ is valid
  mutable u16 fv : 1;  // if true, ft_ is valid

  mutable η0ft             ft_;  // frame type
  η0t                      t_;   // intended type
  union { η0p p; B *s_; }  d;    // primitive or buffered data
  B                       *cs;   // compressed data or null


  B *cdata() const
    { A(sa, "cdata() without sa");
      let r  = new B; r->reserve(ZSTD_compressBound(d.s_->size()));
      let cr = ZSTD_compress(r   ->data(), r   ->capacity(),
                             d.s_->data(), d.s_->size(), c_);
      if (ZSTD_isError(cr))
      { delete r;
        A(0, "η₀ compression error: " << ZSTD_getErrorName(cr)); }
      r->resize(cr);
      return r; }


  void  del_c() {          if ( cs) delete cs,    cs = nullptr;            }
  void  del_s() { del_c(); if ( sa) delete d.s_,  sa = false;              }
  B    &s    () {          if (!sa) d.s_ = new B, sa = true;  return *d.s_; }
  B    &c    () {          if (!cs) cs = cdata();             return *cs;   }
};


#if τdebug_iostream
O &operator<<(O &s, η0o const &x)
{
  return s << "TODO: << η₀o";
}
#endif


}


#include "end.hh"


#endif
