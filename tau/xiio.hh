#ifndef τξio_h
#define τξio_h


#include "xi.hh"
#include "eta.hh"
#include "begin.hh"

namespace τ
{


// A ξ reader, strongly-referenced
struct ξi final
{
  ξi() {}
  ξi(Sp<ξ> x_) : x(x_) {}

  ξi &operator=(ξi const &c) { x = c.x; return *this; }

  operator bool() const { return Sc<bool>(x); }
  Sp<ξ> inner_ξ() const { return x; }

  void     close()       { x.reset(); }
  ηi  operator *() const { A(x,  "*ξi on closed");            return ηi{**x}; }
  ξi &operator++()       { A(x, "++ξi on closed"); x->next(); return *this; }

  bool eof(bool nonblock = false) const { return !x || x->eof(nonblock); }

  ξi &weaken()     { if (x) x->weaken();  return *this; }
  ξi &ensure(uN c) { if (x) x->ensure(c); return *this; }


  struct it
  {
    Sp<ξ> x;  // must hold a strong reference
    ξ::it i;

    bool operator==(it const &y) const { return i == y.i; }
    it  &operator++()                  { ++i; return *this; }
    ηi   operator* () const            { return ηi{*i}; }
  };

  it begin() const { return x ? it{x, x->begin()} : end(); }
  it end()   const { return {nullptr, {nullptr}}; }


protected:
  Sp<ξ> x;
};


// A ξ writer, weakly-referenced
// Note that all operations are const-qualified to simplify usage from
// inside C++ lambdas, but aren't constant with respect to the underlying
// ξ. This is kind of what you'd expect since ξo is a thin wrapper.
//
// TODO: add auto-close when all ξo instances are gone (this requires a
// second pointer, I think; its lifecycle differs from the weak ptr)
struct ξo final
{
  ξo() {}
  ξo(Wp<ξ> x_) : x(x_) {}

  ξo &operator=(ξo const &c) { x = c.x; return *this; }

  operator  bool   ()            const { return !x.expired(); }
  ξo const &ensure (uN c)        const { if (let y = x.lock()) y->ensure(c); return *this; }
  void      close  ()            const { if (let y = x.lock()) y->close(); }
  Sp<ξ>     inner_ξ()            const { return x.lock(); }
  ηo        r      (uN s0 = 256) const { return ηo(x, s0); }


protected:
  Wp<ξ> x;
};


// A full-duplex pair of ξi and ξo, canonically with ξi facing rightwards
// (we can read from the thing to our left) and ξo facing leftwards (we
// can write to the thing on our left).
struct ξd final
{
  ξi f;  // forward channel (left to right)
  ξo b;  // backward channel (right to left)
};


O &operator<<(O&, ξd const&);


}

#include "end.hh"


#endif
