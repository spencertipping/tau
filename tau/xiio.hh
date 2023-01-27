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
  ξi(Sp<ξ> x_) : xs(x_) {}

  void     close()       { xs.reset(); xw.reset(); }
  η0i operator *() const { let x = lock(); A(x,  "*ξi on closed");            return **x; }
  ξi &operator++()       { let x = lock(); A(x, "++ξi on closed"); x->next(); return *this; }
  bool       eof() const
    { if (let x = lock()) return x->eof();
      else                return false; }

  ξi   &weaken()     { xw = xs; xs.reset(); return *this; }
  Sp<ξ> lock() const { return xs ? xs : xw.lock(); }


  struct it
  {
    Sp<ξ> x;  // must hold a strong reference
    ξ::it i;

    bool operator==(it const &y) const { return i == y.i; }
    it  &operator++()                  { ++i; return *this; }
    η0i  operator* () const            { return *i; }
  };

  it end()   const { return {nullptr, {nullptr}}; }
  it begin() const
    { if (let x = lock()) return {x, x->begin()};
      else                return end(); }


protected:
  Sp<ξ> xs;
  Wp<ξ> xw;
};


struct ξo
{
  ξo(Sp<ξ> x_) : x(x_) {}

  void       close()             const { if (let y = x.lock()) y->close(); }
  bool operator<< (η0o const &z) const {     let y = x.lock(); return y && *y << z; }
  bool operator<<=(η0o const &z) const {     let y = x.lock(); return y && (*y <<= z); }

protected:
  Wp<ξ> x;
};


struct ξio
{
  ξio(Λ &l, uN c) : x(new ξ(l, c)), i_(false), o_(false) {}

  uN capacity() const { return x->capacity(); }

  ξi i() { A(!i_, "ξi already claimed"); i_ = true; return ξi{x}; }
  ξo o() { A(!o_, "ξi already claimed"); o_ = true; return ξo{x}; }

protected:
  Sp<ξ> x;
  bool  i_;  // has ξi been claimed?
  bool  o_;  // has ξo been claimed?
};


}

#include "end.hh"


#endif
