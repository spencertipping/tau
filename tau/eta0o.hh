#ifndef τη0o_h
#define τη0o_h


#include "types.hh"
#include "numerics.hh"
#include "eta-types.hh"
#include "eta0i.hh"

#include "begin.hh"

namespace τ
{


// η₀ frame output container, with small-value optimization
struct η0o
{
  η0o(η0t t) : f_(0), c_(0), h_(0), t_(t),
               si_({0}), ss_(0), so_(nullptr), cs_(nullptr) {}

  ~η0o()
    { if (so_) delete so_;
      if (cs_) delete cs_; }


  η0o &operator=(η0i const &i)
    { f(i.f()); c(i.c()); h(i.h()); clear();
      memcpy(at_(0, i.size()), i.data(), i.size());
      return *this; }

  η0o &operator=(η0o const &o)
    { f(o.f()); c(o.c()); h(o.h()); si_ = o.si_; ss_ = o.ss_; clear();
      memcpy(at_(0, o.isize()), o.idata(), o.isize());
      return *this; }


  bool f() const { return f_; }  η0o &f(bool f__) { f_ = f__; touch(); return *this; }
  u8   c() const { return c_; }  η0o &c(u8   c__) { c_ = c__; touch(); return *this; }
  bool h() const { return h_; }  η0o &h(bool h__) { h_ = h__; touch(); return *this; }
  η0t  t() const { return t_; }  η0o &t(η0t  t__) { t_ = t__; touch(); return *this; }


  uN   osize() const { return isize() + fsize(); }
  uN   isize() const { return cs() ? cs()->size() : ssize(); }
  uN   ssize() const { return so_ ? so_->size() : ss_; }

  u8c *idata() const { return cs() ? cs()->data() : sdata(); }
  u8c *sdata() const { return so_ ? so_->data() : si_.data(); }
  u8  *sdata()       { return so_ ? so_->data() : si_.data(); }


  u8  *iptr(uN l)    { return at_(isize(), l); }

  void into(u8*) const;


  // Frame size: size of the frame without its inner data
  uN fsize() const
    { switch (ft())
      {
      case η0ft::s: return 1;
      case η0ft::m: return 2;
      case η0ft::l: return 2 + ubytes(std::max(Sc<uN>(1), isize()));
      case η0ft::d: return 2 + (h_ ? 32 : 0) + ubytes(std::max(Sc<uN>(1), isize()));
      default: A(0, "η₀o ft OOB"); return 0;
      } }


  η0ft ft() const
    { let t = Sc<u8>(t_);
      let s = isize();
      return h_                                              ? η0ft::d
           : c_ || t >= 64 || isize() >= 128                 ? η0ft::l
           : t >= 32 || s != 1 && s != 2 && s != 4 && s != 8 ? η0ft::m
           :                                                   η0ft::s; }


  η0o &clear()
    { touch();
      if (so_) delete so_, so_ = nullptr;
      ss_ = 0;
      return *this; }

  η0o &expand_to(uN c)
    { A(c >= ssize(), "η₀o::expand_to cannot shrink " << ssize() << " to " << c);
      if (c == ssize()) return *this;
      touch();
      if (c > is)
        if (!so_) { (so_ = new B)->resize(c); memcpy(so_->data(), si_.data(), ss_); }
        else      so_->resize(c);
      else ss_ = c;
      return *this; }


protected:
  sletc is = 16;   // number of bytes of inline storage

  u8   f_ : 1;
  u8   c_ : 5;
  u8   h_ : 1;
  η0t  t_;
  Ar<u8, is> si_;  // inline string data
  uN         ss_;  // inline string size
  B         *so_;  // out-of-line string data
  B mutable *cs_;  // compresed string, or null


  void touch() const { if (cs_) delete cs_, cs_ = nullptr; }

  B *cs()    const { return !c_ ? nullptr : cdata(); }
  B *cdata() const;

  u8 *at_(uN i, uN l)
    { if (isize() < i + l) expand_to(i + l);
      return so_ ? so_->data() + i : si_.data() + i; }
};


template<class T> struct η0ot_ { sletc v = false; };

template<> struct η0ot_<η0o> { sletc v = true; };
template<> struct η0ot_<η0i> { sletc v = true; };

// The set of types that can output themselves -- i.e. they provide
// .osize() and .into(u8*)
template<class T> concept η0ot = η0ot_<T>::v;


O &operator<<(O&, η0o const&);


}


#include "end.hh"


#endif
