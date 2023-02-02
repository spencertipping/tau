#ifndef τξio_h
#define τξio_h


#include "xi.hh"
#include "eta.hh"
#include "begin.hh"

namespace τ
{


// A ξ reader, strongly-referenced
struct ξi
{
  ξi() {}
  ξi(Sp<ξ> x_) : x(x_) {}

  ξi &operator=(ξi const &c) { x = c.x; return *this; }


  void     close()       { x.reset(); }
  η0i operator *() const { A(x,  "*ξi on closed");            return **x; }
  ξi &operator++()       { A(x, "++ξi on closed"); x->next(); return *this; }

  bool eof(bool nonblock = false) const { return !x || x->eof(nonblock); }

  ξi &weaken()     { x->weaken(); return *this; }
  ξi &ensure(uN c) { x->ensure(c); return *this; }


  struct it
  {
    Sp<ξ> x;  // must hold a strong reference
    ξ::it i;

    bool operator==(it const &y) const { return i == y.i; }
    it  &operator++()                  { ++i; return *this; }
    η0i  operator* () const            { return *i; }
  };

  it begin() const { return x ? it{x, x->begin()} : end(); }
  it end()   const { return {nullptr, {nullptr}}; }

  ξ &inner_ξ() const { return *x; }


protected:
  Sp<ξ> x;
};


// A ξ writer, weakly-referenced
struct ξo
{
  ξo() {}
  ξo(Sp<ξ> x_) : x(x_) {}

  ξo &operator=(ξo const &c) { x = c.x; return *this; }


  void close() { if (let y = x.lock()) y->close(); }

  template<η0ot T> bool operator<< (T const &z) { return write(z, false); }
  template<η0ot T> bool operator<<=(T const &z) { return write(z, true); }

  template<η0ot T> bool write(T const &z, bool nonblock = false)
    { let y = x.lock(); return y && y->write(z, nonblock); }


  Sp<ξ> inner_ξ() const { return x.lock(); }

  ξo &ensure(uN c) { if (let y = x.lock()) y->ensure(c); return *this; }


protected:
  Wp<ξ> x;
};


// A full-duplex pair
struct ξd
{
  ξd(Λ &l_)                   : l(l_), f_(new ξ(l_, 0)), b_(new ξ(l_, 0)) {}
  ξd(Λ &l_, Sp<ξ> f, Sp<ξ> b) : l(l_), f_(f),            b_(b)            {}

  // Destructively splices this pair along the forward direction, returning
  // the left and right sides of the newly-cut ξ, respectively.
  P<ξi, ξo> xf(Sp<ψ> q, uN cl, uN cr)
    { Sp<ξ> f{new ξ(l, cr)}; f_->oq(q); f->iq(q);
      let l_ = ξi(f_    ).ensure(cl);
      let r_ = ξo(f_ = f).ensure(cr);
      return {l_, r_}; }

  P<ξo, ξi> xb(Sp<ψ> q, uN cl, uN cr)
    { Sp<ξ> b{new ξ(l, cr)}; b_->oq(q); b->iq(q).weaken();
      let l_ = ξo(b_    ).ensure(cl);
      let r_ = ξi(b_ = b).ensure(cr);
      return {l_, r_}; }

  Sp<ξ> f() { return f_; }
  Sp<ξ> b() { return b_; }


  ξo fo() { return ξo(f_); }  // →
  ξi bi() { return ξi(b_); }  // →
  ξi fi() { return ξi(f_); }  // ←
  ξo bo() { return ξo(b_); }  // ←


protected:
  Λ     &l;
  Sp<ξ>  f_;
  Sp<ξ>  b_;
};


}

#include "end.hh"


#endif
