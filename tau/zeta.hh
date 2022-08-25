#ifndef tau_flux_ζ_h
#define tau_flux_ζ_h


#include <algorithm>


#include "arch.hh"
#include "types.hh"
#include "numerics.hh"

#include "Lambda.hh"
#include "gate.hh"


#include "begin.hh"

namespace tau
{


#if tau_wordsize == 64
  uf8c constexpr ζb0 = 16;
#elif tau_wordsize == 32
  ut8c constexpr ζb0 = 12;
#else
# error unsupported word size for ζ
#endif


typedef u8      *ζp;
typedef ζp const ζpc;

constexpr uNc ζω  = Nl<uN>::max();
constexpr ζpc ζωp = Nl<ζp>::max();


struct ζb
{
  uNc c;             // invariant: c is a power of 2
  ζp  xs = nullptr;  // invariant: xs[0..c-1] are allocated
  uN  ri = 0;
  uN  wi = 0;
  uN  ci = 0;

  ζb(uf8 b_) : c(Sc<uN>(1) << b_) { xs = new u8[c]; }
  ~ζb()                           { delete[] xs; }

  ζp   operator+ (uN a) const { return xs + a; }
  bool operator[](uN a) const { return wr() ? a < wi || a >= ri && a < ci
                                            : a >= ri && a < wi;}

  bool wr()     const { return ri > wi; }
  uN   bp(uN a) const { return wr() && a >= ci ? a - ci : a; }
  uN   ra()     const { return wr() ? wi + (ci - ri) : wi - ri; }
  uN   wa()     const { return wr() ? ri - wi        : std::max(c - wi, ri); }

  void free(uN a)
    { if (!wr()) A(a <= wi && a >= ri,            "a = " << a << ", ri = " << ri << ", wi = " << wi);
      else     { A(a <= wi || a >= ri && a <= ci, "a = " << a << ", ri = " << ri << ", wi = " << wi << ", ci = " << ci);
                 if (a == ci) a = 0; }
      ri = a; }

  uN alloc(uN s)
    { if      (s > wa())                                                return ζω;
      else if (!wr() && s + wi > c) {    ci = wi; wi  = s; ri = bp(ri); return  0; }
      else                          { let a = wi; wi += s;              return  a; } }
};


struct ζ
{
  ζb     b;
  λg     rg;
  λg     wg;
  bool   rc {false};
  bool   wc {false};

  ζ(ζ &x) = delete;
  ζ(Λ &l, uf8 b_ = ζb0) : b(b_), rg{l}, wg{l} {}


  ζ &rω() { rc = true; wg.w(); return *this; }
  ζ &wω() { wc = true; rg.w(); return *this; }


  ζp operator+(uN a) const { return b + a; }
  uN         a()     const { return b.ri; }
  bool      ra()     const { return b.ra(); }
  bool      wa()     const { return b.wa(); }
  bool      ri()     const { return !wc || ra(); }
  bool      wi()     const { return !rc; }


  bool wra()     { while (!b.ra())    { if (wc) return false; rg.y(λs::I); }; return true; }
  bool wwa(uN s) { while (b.wa() < s) { if (rc) return false; wg.y(λs::O); }; return true; }


  template<class R>
  ζp r()
    { if (!wra()) return ζωp;
      let a = b + b.ri;
      b.free(b.ri + R::size_of(a));
      wg.w();
      return a; }


  template<class W>
  bool w(W const &x)
    { let s = x.size(b.c, b.wa());
      uN  a;
      while ((a = b.alloc(s)) == ζω) { if (rc) return false; wg.y(λs::O); }
      x.write(b + a, s);
      rg.w();
      return true; }
};


#if tau_debug_iostream
O &operator<<(O &s, ζb const &b)
{
  return s << "ζb[c=" << b.c << " ri=" << b.ri
           << " wi=" << b.wi << " ci=" << b.ci
           << " ra=" << b.ra() << " wa=" << b.wa() << "]";
}

O &operator<<(O &s, ζ const &x)
{
  return s << "ζ" << (x.rc ? "#" : "r") << (x.wc ? "#" : "w") << " " << x.b;
}
#endif


}

#include "end.hh"


#endif
