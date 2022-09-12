#ifndef tau_ζ_h
#define tau_ζ_h


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
  uf8c constexpr ζb0 = 14;
#else
# error unsupported word size for ζ
#endif


#if tau_debug_iostream
  struct ζb;
  template<class R> struct ζ;
  O &operator<<(O &s, ζb const &b);
  template<class R> O &operator<<(O &s, ζ<R> const &x);
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
  uN  ci = 0;        // TODO: use this to indicate wrap so we get full c

  ζb(uf8 b_) : c(Sc<uN>(1) << b_) { A(c, "ζb uN overflow, b = " << b_); xs = new u8[c]; }
  ~ζb()                           { delete[] xs; }

  ζp   operator+ (uN a) const { return xs + a; }
  bool operator[](uN a) const { return wr() ? a < wi || a >= ri && a < ci
                                            : a >= ri && a < wi; }

  bool wr()     const { return ri > wi; }
  uN   bp(uN a) const { return wr() && a >= ci ? a - ci : a; }
  uN   ra()     const { return wr() ? wi + (ci - ri) : wi - ri; }
  uN   wa()     const { return wr() ? ri - wi - 1    : ri ? std::max(c - wi, ri - 1) : c - wi; }

  void free(uN a)  // free all memory up to specified address
    { if (!wr()) A(a <= wi && a >= ri,            "a = " << a << ", ri = " << ri << ", wi = " << wi);
      else     { A(a <= wi || a >= ri && a <= ci, "a = " << a << ", ri = " << ri << ", wi = " << wi << ", ci = " << ci);
                 if (a == ci) a = 0; }
      ri = a;
      if (ri == wi) ri = wi = 0; }

  uN alloc(uN s)
    { if      (s > wa())                                                return ζω;
      else if (!wr() && s + wi > c) {    ci = wi; wi  = s; ri = bp(ri); return  0; }
      else                          { let a = wi; wi += s;              return  a; } }

  void rewind(uN a) { wi -= a; }
};


template<class R = void>
struct ζ
{
  sletc frc = 0x1;
  sletc fwc = 0x2;
  sletc fl  = 0x4;

  ζb  b;
  uf8 fs;
  λg  rg;
  λg  wg;
  Mu  l;

  ζ(ζ &x) = delete;
  ζ(Λ &l, uf8 b_ = ζb0, uf8 fs_ = 0) : b(b_), fs(fs_), rg{l}, wg{l} {}


  ζ &rω() { fs |= frc; wg.w(); return *this; }
  ζ &wω() { fs |= fwc; rg.w(); return *this; }


  ζp operator+(uN a) const { return b + a; }
  uN         a()     const { return b.ri; }
  bool      ra()     const { return b.ra(); }
  bool      wa()     const { return b.wa(); }
  bool      ri()     const { return !wc() || ra(); }
  bool      wi()     const { return !rc(); }
  bool      rc()     const { return fs & frc; }
  bool      wc()     const { return fs & fwc; }
  bool      li()     const { return fs & fl; }


  bool wra()     { while (!b.ra())    { if (wc()) return false; rg.y(λs::I); }; return true; }
  bool wwa(uN s) { while (b.wa() < s) { if (rc()) return false; wg.y(λs::O); }; return true; }


  ζp   operator*() { return wra() ? b + b.ri : ζωp; }
  bool operator++()
    { if (!ra()) return false;
      if (li()) l.lock();
      R::free(b + b.ri);
      b.free(b.ri + R::size_of(b + b.ri));
      if (li()) l.unlock();
      wg.w();
      return true; }


  template<class W>
  bool operator<<(W const &x)
    { let s = x.size();

      // NOTE: ζb reserves one byte when wrapped to mark it wrapped state (see ζb::wa).
      A(s <= b.c - 1, s << "[s] ≥ " << b.c << "[ζb.c]");
      while (s > b.wa()) { if (rc()) return false; wg.y(λs::O); }

      if (li()) l.lock();
      uNc a = b.alloc(s);

      // write() returns the _actual_ size written, or 0 if it's the same
      // as .size(), or ζω if nothing was written.
      if (let w = x.write(b + a))
      { if (w == ζω) { b.rewind(s); if (li()) l.unlock(); return false; }
        b.rewind(s - w); }
      if (li()) l.unlock();

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

template<class R>
O &operator<<(O &s, ζ<R> const &x)
{
  return s << "ζ" << (x.li() ? "L" : "") << (x.rc() ? "#" : "r") << (x.wc() ? "#" : "w") << " " << x.b;
}
#endif


}

#include "end.hh"


#endif
