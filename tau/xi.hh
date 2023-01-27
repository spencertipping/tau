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


// η pipe: a circular buffer with Λ-mediated read/write negotiation
// and endpoint close tracking. We also allocate one-off buffers for
// values larger than the ζ capacity.
//
// ξs will be deleted if nobody wants to read from them, and .close()
// will be called when the writer is done.
struct ξ
{
  ξ(Λ &l, uN c) : z(c), b(nullptr), t(0), wc(false), rg{l}, wg{l} {}

  // By this point our instance state won't be accessible to any λs, so
  // all messaging must go through the wake-gates; false means the ξ is
  // closed, so assume it no longer exists.
  ~ξ() { rg.w(false); wg.w(false); }

  uN ra() const { return b ? sb : z.ra(); }
  uN wa() const { return z.wa(); }
  uN wt() const { return t; }  // total bytes written

  bool eof()
    { while (!ra() && !wc) if (!rg.y(λs::ξR)) return true;
      return !ra() && wc; }


  // Writer interface: close() to finalize the write-side, << to write
  // a value
  void close() { wc = true; rg.w(false); }

  bool operator<<(η0o const &x)   // blocking write (false on epipe)
    { let s = x.osize();
      if (s > z.capacity()) return sidecar_write(x, s, false);
      else                  return pipe_write   (x, s, false); }

  bool operator<<=(η0o const &x)  // nonblocking write
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
      if (b) b = nullptr, sb = 0;
      else   z.free((*(*this)).osize());
      wg.w(true); }  // we are now writable


  struct it
  {
    ξ *const y;  // null for EOF

    bool        eof()            const { return !y || y->eof(); }
    bool operator==(it const &x) const { return eof() == x.eof(); }
    it  &operator++()       { A(y, "++ξ::end"); y->next(); return *this; }
    η0i  operator* () const { A(y, "*ξ::end");             return **y; }
  };

  it begin() { return {this}; }
  it end()   { return {nullptr}; }


protected:
  ζ        z;
  u8      *b;   // sidecar buffer for large values
  uN       sb;  // sizeof(*b)
  u64      t;   // total bytes written
  bool     wc;  // writer has closed; all blocking reads will fail
  λg<bool> rg;  // read-gate; false means insta-bail
  λg<bool> wg;  // write-gate; false means insta-bail

#if τdebug_iostream
  friend O &operator<<(O&, ξ const&);
#endif


  bool pipe_write(η0o const &x, uN s, bool nonblock)
    { while (!write(z.iptr(s), x, s))
        if (nonblock || !wg.y(λs::ξW)) return false;
      return true; }

  bool sidecar_write(η0o const &x, uN s, bool nonblock)
    { while (b)
        if (nonblock || !wg.y(λs::ξW)) return false;
      return write(b = new u8[s], x, sb = s); }

  bool write(u8 *i, η0o const &x, uN s)
    { if (!i) return false;
      t += s;
      x.into(i);
      rg.w(true);
      return true; }
};


#if τdebug_iostream
O &operator<<(O &s, ξ const &y)
{
  return s << "ξ[" << (y.wc ? "#" : "") << "wt=" << y.wt() << " " << y.z << "]";
}
#endif


}

#include "end.hh"


#endif
