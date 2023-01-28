#ifndef τξio_h
#define τξio_h


#include "xi.hh"
#include "eta.hh"
#include "begin.hh"

namespace τ
{


struct ξio;  // a Sp<ξ> container that tracks when read/write are claimed
struct ξi;   // a ξ reader, strongly-referenced
struct ξo;   // a ξ writer, weakly-referenced


struct ξi
{
  ξi() {}
  ξi(Sp<ξ> x_) : x(x_) {}

  ξi &operator=(ξi const &c) { x = c.x; return *this; }


  void     close()       { x.reset(); }
  η0i operator *() const { A(x,  "*ξi on closed");            return **x; }
  ξi &operator++()       { A(x, "++ξi on closed"); x->next(); return *this; }
  bool       eof() const {                                    return !x || x->eof(); }
  uN          ra() const {                                    return x ? x->ra() : 0; }

  ξi &weaken() { x->weaken(); return *this; }


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


struct ξo
{
  ξo() {}
  ξo(Sp<ξ> x_) : x(x_) {}

  ξo &operator=(ξo const &c) { x = c.x; return *this; }


  void                        close()           const { if (let y = x.lock()) y->close(); }
  template<η0ot T> bool operator<< (T const &z) const {     let y = x.lock(); return y && *y << z; }
  template<η0ot T> bool operator<<=(T const &z) const {     let y = x.lock(); return y && (*y <<= z); }

  Sp<ξ> inner_ξ() const { return x.lock(); }


protected:
  Wp<ξ> x;
};


struct ξio
{
  ξio(Λ &l, uN c) : x(new ξ(l, c)), i_(false), o_(false) {}
  ~ξio() { A(!o_ || i_, "ξ→ has no input"); }

  uN capacity() const { return x->capacity(); }

  ξi i(Sp<ψ> q) { A(!i_, "ξi already claimed"); i_ = true; x->oq(q); return ξi{x}; }
  ξo o(Sp<ψ> q) { A(!o_, "ξi already claimed"); o_ = true; x->iq(q); return ξo{x}; }

  bool can_i() const { return !i_; }
  bool can_o() const { return !o_; }

  ξ &inner_ξ() const { return *x; }


protected:
  Sp<ξ> x;
  bool  i_;  // has ξi been claimed?
  bool  o_;  // has ξo been claimed?

  friend O &operator<<(O&, ξio const&);
};


}

#include "end.hh"


#endif
