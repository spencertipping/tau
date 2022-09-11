#ifndef tau_ϝ_h
#define tau_ϝ_h


#include <functional>


#include "types.hh"
#include "gamma.hh"

#include "begin.hh"

namespace tau
{


// Hex-packed ϝ configuration; each four-bit digit configures a φ.
// Bit layout looks like this:
//
// 0x εδβα οι  <- four φs, two IO specifiers
//
// Each digit specifies the state of the φ along three dimensions:
//
// _4__  <- if set, the φ will be a server
// __2_  <- if set, the φ will be read-only (not writable)
// ___1  <- if set, the φ will be write-only (not readable)
//
// ι and ο specify which φ is used for default reading and default
// writing, respectively. Each of these is a number from 0 to 3, inclusive,
// where 0 = α and 3 = ε.

typedef u32 ϝξ;


struct ϝ
{
  sletc ξι = 0x001210;
  sletc ξϊ = 0x000010;
  sletc ξΦ = 0x003000;
  sletc ξβ = 0x004010;
  sletc ξτ = 0x213000;
  sletc ξγ = 0x004311;
  sletc ξδ = 0x000110;

  γ        g;
  ϝξ const c;
  uN       r{0};

  template<class... Fs>
  ϝ(Φ &f_, ϝξ c_, Fs... fs) : g{f_}, c(c_)
    { φc(c >> 8);
      φc(c >> 12);
      φc(c >> 16);
      φc(c >> 20);
      λcs(fs...); }


  template<class F, class... Fs>
  void λcs(F f, Fs... fs) { λc(f); λcs(fs...); }
  void λcs() {}


  λi λc(F<void(ϝ&)> &&f)
    { return g.λc([&, f = std::move(f)]()
      { ++r;
        f(*this);
        if (!--r) delete this;
        return 0; }); }

  λi λc(F<void(ϝ&, γ&)> &&f)
    { return g.λc([&, f = std::move(f)]()
      { ++r;
        f(*this, g);
        if (!--r) delete this;
        return 0; }); }

  φi φc(ϝξ x)    { return g.φc(ζb0, x & 4, x & 1, x & 2); }

  φi ιi()  const { return c & 3; }
  φi οi()  const { return c >> 4 & 3; }

  γφ &φι() const { return g[ιi()]; }
  γφ &φο() const { return g[οi()]; }
  γφ &α()  const { return g[0]; }
  γφ &β()  const { return g[1]; }
  γφ &δ()  const { return g[2]; }
  γφ &ε()  const { return g[3]; }

  γφ &operator[](φi i) const { return g[i]; }

  template<class T> ϝ &Θ(T t) { g.Θ(t); return *this; }

  γφ::it begin() const { return φι().begin(); }
  γφ::it end()   const { return φι().end(); }

  template<class T> bool operator<< (T const &x) { return φο() <<  x; }
  template<class T> bool operator<<=(T const &x) { return φο() <<= x; }

  template<class T>
  typename std::enable_if<iti<T>::v, bool>::type
  operator<(T const &x) { return x > φο(); }

  ϝ &operator|(ϝ &x) { g(οi(), x.g, x.ιi()); return x; }
  ϝ &operator&(ϝ &x) { g(3,    x.g, x.ιi()); return x; }
  ϝ &operator^(ϝ &x) { g(οi(), x.g, 2);      return x; }
};


// important: prevents attempted coercion of Φf into an o9
template<class O>
ic bool operator<<(ϝ &f, Φf<O> &x) { return f.φο() << x; }


}

#include "end.hh"


#endif
