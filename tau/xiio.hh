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

  Sp<ξ> inner_ξ() const { return x; }


protected:
  Sp<ξ> x;
};


// A ξ writer, weakly-referenced
// Note that all operations are const-qualified to simplify usage from
// inside C++ lambdas, but aren't constant with respect to the underlying
// ξ. This is kind of what you'd expect since ξo is a thin wrapper.
struct ξo
{
  ξo(Λ &l_, Wp<ξ> x_) : x(x_), l(l_), c(0), p(nullptr) {}

  ξo &operator=(ξo const &c)           { x = c.x; return *this; }

  operator  bool   ()            const { return !x.expired(); }
  ξo const &ensure (uN c)        const { if (let y = x.lock()) y->ensure(c); return *this; }
  void      close  ()            const { if (let y = x.lock()) y->close(); }
  Sp<ξ>     inner_ξ()            const { return x.lock(); }
  ηo        r      (uN s0 = 256) const { return ηo(x, ref(), s0); }


protected:
  Wp<ξ>        x;
  Λ           &l;  // scheduler
  u64 mutable  c;  // number of context switches
  ξ   mutable *p;  // cached referent of x

  ξ *ref() const
    { if (l.cs() != c) c = l.cs(), p = x.lock().get();
      return p; }
};


// TODO: new ξd struct


}

#include "end.hh"


#endif
