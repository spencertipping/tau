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


struct ξ;    // η pipe
struct ξi;   // ξ input (reader)
struct ξo;   // ξ output (writer)
struct ξio;  // stateful ξ assembler


// η pipe: a circular buffer with Λ-mediated read/write negotiation
// and endpoint close tracking. We also allocate one-off buffers for
// values larger than the ζ capacity.
struct ξ
{
  ξ(Λ &l, uN c) : z(c), b(nullptr), t(0),
                  rc(false), wc(false), rg{l}, wg{l} {}

  // FIXME: big problem here: we can't wake λs from destructor because
  // by the time the λs run, this object will no longer exist. That means
  // any finalization must happen (and the λs finish) _before_ ξs are
  // destroyed -- or we just kill those λs, but that seems wrong.
  //
  // NOTE: to be correct, λ gates must hold shared pointers to ξs;
  // otherwise we have no guarantee that ξ will outlive the λ scheduling
  // delay.
  //
  // NOTE: revised λg definition will suffice here; we just need to send
  // a message to bail or to continue in the w() call
  ~ξ()
    { rc = wc = true;
      rg.w();
      wg.w(); }

  // FIXME: eof() should be blocking
  bool eof()   const { return wc && !ra(); }
  bool epipe() const { return rc; }
  uN   ra()    const { return z.ra(); }
  uN   wa()    const { return z.wa(); }

  // Total bytes written through this ξ
  uN wt() const { return t; }


  // Writer interface: close() to finalize the write-side, << to write
  // a value
  ξ &close() { rc = true; rg.w(); return *this; }

  // Blocking write
  bool operator<<(η0o const &x)
    { let s = x.osize();
      u8 *p;
      while (!epipe() && !(p = z.iptr(s))) wg.y(λs::ξW);
      if (epipe()) return false;
      t += s;
      x.into(p);
      return true; }


  η0i operator*() const
    { A(ra(), "*ξ with !ra(), " << z);
      return η0i(z + 0); }


  struct it
  {
    ξ    &y;
    bool  eof;

    η0i  operator* () const { return *y; }
    it  &operator++()       { y.z.free((*(*this)).osize()); return *this; }
    bool operator==(it const &x) const
      {
      }
  };



protected:
  ζ    z;
  u8  *b;   // sidecar buffer for large values
  u64  t;
  bool rc;  // reader has closed; all writes will fail
  bool wc;  // writer has closed; all blocking reads will fail
  λg   rg;
  λg   wg;

#if τdebug_iostream
  friend O &operator<<(O&, ξ const&);
#endif


  // Write to pipe or custom buffer

};


#if τdebug_iostream
O &operator<<(O &s, ξ const &y)
{
  return s << "ξ" << (y.eof() ? "#" : " ") << y.z;
}
#endif


}

#include "end.hh"


#endif
