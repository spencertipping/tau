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
//
// NOTE: primitives cannot be hashed or compressed; this makes sense
// because you would never want to do this.
//
// NOTE: in the future we can have this just handle size, delegate
// contents to higher-level η₁ classes
struct η0o
{
  η0o() : c_(0), h_(false), f_(false), fv(false), t_(η0t::uint_be), d({{0}}), cs(nullptr) {}
  η0o(η0o &&x) { *this = std::move(x); }

  template<η0at T>
  η0o(T x)
    : c_(0), h_(false), f_(false), fv(false), t_(η0t::uint_be), cs(nullptr)
    { t(η0at_<T>::t); *this = x; }

  ~η0o() { t(η0t::uint_be); }  // Convert to primitive to force resources to be freed


  // NOTE: works only if the other η₀o is in a valid state
  η0o &operator=(η0o &&x)
    { c_  = x.c_;  h_ = x.h_; f_ = x.f_; fv = x.fv;
      ft_ = x.ft_; t_ = x.t_; d  = x.d;  cs = x.cs;
      x.cs = nullptr; x.t_ = η0t::uint_be;  // transfer ownership
      return *this; }

  // TODO: operator=(η0i)

  η0o &operator=(int   x) { t(η0t::int_be);   d.p.i = x; return *this; }

  η0o &operator=(u64   x) { t(η0t::uint_be);  d.p.u = x; return *this; }
  η0o &operator=(i64   x) { t(η0t::int_be);   d.p.i = x; return *this; }
  η0o &operator=(f64   x) { t(η0t::float_be); d.p.f = x; return *this; }
  η0o &operator=(bool  x) { t(η0t::boolean);  d.p.b = x; return *this; }
  η0o &operator=(void *x) { t(η0t::η0);       d.p.p = x; return *this; }

  η0o &operator=(Stc &x) { t(η0t::utf8);  sd() = Bv{Rc<u8c*>(x.data()), x.size()}; return *this; }
  η0o &operator=(Bc  &x) { t(η0t::bytes); sd() = x;                                return *this; }
  η0o &operator=(B  &&x) { t(η0t::bytes); sd() = std::move(x);                     return *this; }


  η0o &c(u8 c__)   { A(!(c_ = c__) || !η0tp[t_], "cannot compress primitives"); return *this; }
  η0o &h(bool h__) { A(!(h_ = h__) || !η0tp[t_], "cannot hash primitives");     return *this; }
  η0o &f(bool f__) { A(!(f_ = f__) || !η0tp[t_], "cannot flag primitives");     return *this; }
  η0o &t(η0t t__)
    { if (η0tp[t__])
      { c_ = 0, h_ = false, f_ = false;
        if (!p()) del_c(), delete d.s_; }   // transition to primitive
      else
        if (p()) d.s_ = new B;              // transition to buffered
      t_ = t__;
      fv = false;
      return *this; }

  u8   c() const { return c_; }
  bool h() const { return h_; }
  bool f() const { return f_; }
  η0t  t() const { return t_; }
  bool p() const { return η0tp[t_]; }  // is our type a primitive

  // NOTE: append() and iptr() both delete any compressed data
  η0o &append(u8c *d, uN l) { A(!p(), "cannot append to " << t_); sd().append(d, l);            touch(); return *this; }
  u8  *iptr  (uN l)
    { A(!p(), "cannot get iptr for " << t_);
      touch();
      auto &s = sd();
      s.reserve(s.size() + l);
      let r = s.data() + s.size();
      s.resize(s.size() + l, 0);
      return r; }


  // Inner size of the fully-encoded frame data: that is, the size that
  // the frame will end up describing.
  uN isize() const
    { switch (t_)
      {
      case η0t::η0:       return sizeof(d.p.p);
      case η0t::boolean:  return 1;
      case η0t::uint_be:  return su(d.p.u);
      case η0t::int_be:   return si(d.p.i);
      case η0t::float_be: return sizeof(d.p.f);
      default:
        return c_ ? 8 + cd().size() : sd().size();
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

  // Serializes the value into the output buffer, which must be at least
  // as large as osize(). Note that osize() will change if you modify the
  // contents of this η₀o.
  void into(u8 *m) const
    { let s = isize();
      let b = ubytes(std::max(Sc<uN>(1), s));
      uN  o = 0;

      switch (ft())
      {
      case η0ft::s:
        m[o++] = Sc<u8>(t_) << 2 | ubits(s);
        break;
      case η0ft::m:
        m[o++] = 0x80 | Sc<u8>(t_);
        m[o++] = (f_ ? 0x80 : 0) | s;
        break;
      case η0ft::l:
        m[o++] = 0xc0 | f_ << 4 | (c_ > 0) << 3 | b - 1;
        m[o++] = Sc<u8>(t_);
        for (u8 i = 0; i < b; ++i) m[o++] = s >> (b - i - 1) * 8 & 255;
        break;
      case η0ft::d:
        m[o++] = 0xe0 | h_ << 4 | (c_ > 0) << 3 | b - 1;
        m[o++] = Sc<u8>(t_);
        for (u8 i = 0; i < b; ++i) m[o++] = s >> (b - i - 1) * 8 & 255;
        break;
      }

      if (h_)
      { auto sha3_256 = picosha3::get_sha3_generator<256>();
        Ar<u8, 32> hv{};
        sha3_256(data().begin(), data().end(), hv.begin(), hv.end());
        memcpy(m + o, hv.data(), 32);
        o += 32; }

      if (c_)
      { W(m, o, Sc<u64>(sd().size()));
        o += 8; }

      switch (t_)
      {
      case η0t::η0:       *Rc<void**>(m + o) = d.p.p; o += sizeof(void*);                   break;
      case η0t::int_be:   for (u8 i = 0; i < s; ++i) m[o++] = d.p.i >> (s - i - 1)*8 & 255; break;
      case η0t::uint_be:  for (u8 i = 0; i < s; ++i) m[o++] = d.p.u >> (s - i - 1)*8 & 255; break;
      case η0t::float_be: W(m, o, d.p.f); o += sizeof(d.p.f);                               break;
      case η0t::boolean:  W(m, o++, Sc<u8>(d.p.b ? 1 : 0));                                 break;

      default:
      { let x = data();
        memcpy(m + o, x.data(), x.size());
        o += x.size();
        break; }
      }

      let os = osize();
      A(o == os, "η0o::into() size mismatch: " << o << " ≠ " << os); }


  η0ft ft() const
    { if (!fv)
      { let s = isize();
        let t = Sc<u8>(t_);
        A(!h_ || !f_, "flagged η₀ cannot be hashed");
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
  mutable u16 fv : 1;  // if true, ft_ is valid

  mutable η0ft             ft_;  // frame type
  η0t                      t_;   // intended type
  union { η0p p; B *s_; }  d;    // primitive or buffered data
  mutable B               *cs;   // compressed data or null


  // Called after data has been modified; erases cached state
  void touch() { del_c(); fv = false; }


  // Verbatim or compressed data to be written into the output after
  // the header
  Bc   &data () const { return c_ ? cd() : sd();                    }
  B    &sd   () const { A(!p(), "sd() on primitive"); return *d.s_; }
  B    &cd   () const { if (!cs) cs = cdata();        return *cs;   }
  void  del_c()       { if ( cs) delete cs, cs = nullptr;           }

  B    *cdata() const
    { A(!p(), "cdata() on primitive");
      let r  = new B; r->resize(ZSTD_compressBound(d.s_->size()));
      let cr = ZSTD_compress(r   ->data(), r   ->capacity(),
                             d.s_->data(), d.s_->size(), c_);
      if (ZSTD_isError(cr))
      { delete r;
        A(0, "η₀ compression error: " << ZSTD_getErrorName(cr)); }
      r->resize(cr);
      return r; }
};


inline η0o &operator<<(η0o &o, η0o const &x) { x.into(o.iptr(x.osize())); return o; }

inline η0o &operator<<(η0o &o, Bc &x)
{ return η0tc[o.t()]
       ? o << η0o(x)
       : o.append(x.data(), x.size()); }

inline η0o &operator<<(η0o &o, Stc &x)
{ return η0tc[o.t()]
       ? o << η0o(x)
       : o.append(Rc<u8c*>(x.data()), x.size()); }

template<η0atp T>
η0o &operator<<(η0o &o, T x) { return o << η0o(x); }



#if τdebug_iostream
O &operator<<(O &s, η0o const &x)
{
  return s << "TODO: << η₀o";
}
#endif


}


#include "end.hh"


#endif
