#ifndef τξ_h
#define τξ_h


#include "eta.hh"
#include "zeta.hh"
#include "types.hh"
#include "gate.hh"
#include "Lambda.hh"

#include "begin.hh"

namespace τ
{


struct ψ;


// η pipe: a circular buffer with Λ-mediated read/write negotiation
// and endpoint close tracking. We also allocate one-off buffers for
// values larger than the ζ capacity.
//
// ξs will be deleted if nobody wants to read from them, and .close()
// will be called when the writer is done.
struct ξ
{
  ξ(Λ &l, uN c)
    : z(c), b(nullptr), sb(0), t(0), wc(false), w(false), rg(l), wg(l) {}

  // By this point our instance state won't be accessible to any λs, so
  // all messaging must go through the wake-gates; false means the ξ is
  // closed, so assume it no longer exists.
  ~ξ()
    { rg.w(false); wg.w(false);
      if (b) delete[] b; }

  uN capacity() const { return z.capacity(); }
  uN ra()       const { return b ? sb : z.ra(); }
  uN wa()       const { return z.wa(); }
  uN wt()       const { return t; }   // total bytes written
  uN extra()    const { return sb; }  // extra bytes allocated right now

  bool eof()
    { while (!ra() && !wc) if (!rg.y(λs::ξR)) return true;
      return !ra() && wc; }


  // Writer interface: close() to finalize the write-side, << to write
  // a value
  void close() { wc = true; rg.w(true); }

  // Blocking write, false if epipe
  template<η0ot T> bool operator<<(T const &x)
    { let s = x.osize();
      if (s > z.capacity()) return sidecar_write(x, s, false);
      else                  return pipe_write   (x, s, false); }

  // Nonblocking write, false if epipe or insufficient space now
  template<η0ot T> bool operator<<=(T const &x)
    { let s = x.osize();
      if (s > z.capacity()) return sidecar_write(x, s, true);
      else                  return pipe_write   (x, s, true); }


  // Reader interface: delete ξ to finalize the read-side, iterator and
  // unary */next() to read η0s
  η0i operator*() const
    { A(ra(), "*ξ with !ra(), " << z);
      return !b ? η0i(z + 0) : η0i(b); }

  void next()
    { A(ra(), "++ξ with !ra(), " << z);
      if (b) delete[] b, b = nullptr, sb = 0;
      else   z.free((*(*this)).osize());
      wg.w(true); }  // we are now writable


  struct it
  {
    ξ *const y;  // null for EOF

    bool        eof()            const { return !y || y->eof(); }
    bool operator==(it const &x) const { return eof() == x.eof(); }
    it  &operator++()       { A(y, "++ξ::end"); y->next(); return *this; }
    η0i  operator* () const { A(y,  "*ξ::end");            return **y; }
  };

  it begin() { return {this}; }
  it end()   { return {nullptr}; }


  // Endpoint management
  ξ &iq(Sp<ψ> x) { iqs = x; if (w) weaken(); return *this; }
  ξ &oq(Sp<ψ> x) { oqw = x;                  return *this; }

  Sp<ψ> iq() { return iqs ? iqs : iqw.lock(); }
  Sp<ψ> oq() { return             oqw.lock(); }

  // Weaken reference to generating ψ, e.g. for backflowing ξ
  ξ &weaken() { w = true; iqw = iqs; iqs.reset(); return *this; }


protected:
  ζ        z;
  u8      *b;    // sidecar buffer for large values
  uN       sb;   // sizeof(*b)
  u64      t;    // total bytes written
  bool     wc;   // writer has closed; all blocking reads will fail
  bool     w;    // weaken() has been called, always weaken source ψ
  λg<bool> rg;   // read-gate; false means insta-bail
  λg<bool> wg;   // write-gate; false means insta-bail
  Sp<ψ>    iqs;  // input (writer) ψ -- default strong reference
  Wp<ψ>    iqw;  // input ψ -- optional weak reference
  Wp<ψ>    oqw;  // output (reader) ψ


  friend O &operator<<(O&, ξ const&);


  template<η0ot T> bool pipe_write(T const &x, uN s, bool nonblock)
    { while (!write(z.iptr(s), x, s))
        if (nonblock || !wg.y(λs::ξW)) return false;
      return true; }

  template<η0ot T> bool sidecar_write(T const &x, uN s, bool nonblock)
    { while (ra() || b)
        if (nonblock || !wg.y(λs::ξW)) return false;
      return write(b = new u8[s], x, sb = s); }

  template<η0ot T> bool write(u8 *i, T const &x, uN s)
    { if (!i) return false;
      t += s;
      x.into(i);
      rg.w(true);
      return true; }
};


O &operator<<(O &s, ξ const &y)
{
  return s << "ξ[" << (y.wc ? "#" : "")
           << "wt=" << y.wt() << " " << y.z << "]";
}


}

#include "end.hh"


#endif
