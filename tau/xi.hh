#ifndef τξ_h
#define τξ_h


#include "zeta.hh"
#include "types.hh"
#include "gate.hh"
#include "Lambda.hh"

#include "begin.hh"

namespace τ
{


struct ψ_;
struct ξ;


void ξc_(ξ*);
void ξd_(ξ*);
uN   ξn();

#if τtrack_ξ
S<ξ*> &ξs();
#endif


// η pipe: a circular buffer with Λ-mediated read/write negotiation
// and endpoint close tracking. We auto-expand the buffer for large
// values. (Note that ξ can carry non-η values too; it's data-agnostic.)
//
// ξs will be deleted if nobody wants to read from them, and writers
// can call .close() to indicate that no further values will be
// written. This causes the reader to see eof() == true once remaining
// data is consumed.
//
// ξ values are framed with a native-endian length.
//
// NOTE: don't create ξs directly; use τe::pipe(), which wraps with
// ξi and ξo and does proper memory management.
struct ξ final
{
  ξ(Λ &l, uN c)
    : z(c), wt(0), s(nullptr), wc(false), w(false), rg(l), wg(l)
    { ξc_(this); }

  // By this point our instance state won't be accessible to any λs, so
  // all messaging must go through the wake-gates; false means the ξ is
  // closed, so assume it no longer exists.
  ~ξ()
    { rg.w(false); wg.w(false);
      ξd_(this); }


  uN capacity() const { return z.capacity(); }
  uN ra()       const { return z.ra(); }
  uN wa()       const { return z.wa(); }

  ξ &resize(uN c)     { z.resize(c); return *this; }
  ξ &ensure(uN c)     { z.ensure(c); return *this; }

  // If true, no more values will ever be available to read from this ξ
  bool eof(bool nonblock = false)
    { while (!ra() && !wc) if (nonblock || !rg.y(λs::ξR)) return true;
      return !ra() && wc; }


  // Writer interface: close() to finalize the write-side,
  // iptr() and commit() to write values (or abort() to cancel a write)
  //
  // To write a value, first call iptr() with an upper bound on the size you
  // want to use, then call commit() or abort()

  // Closing the writer means that the reader no longer depends on it;
  // so the writer is immediately up for deallocation. We do this by
  // weakening the input reference.
  void close() { wc = true; rg.w(true); weaken(); }

  Sn<u8> iptr(uN size, bool nonblock = false)
    { A(!s,   "ξ: iptr(" << size << ") with uncommitted value");
      A(size, "ξ: iptr() on size = 0");
      ensure(size + uNs);
      return reserve(size, nonblock); }

  void abort()
    { A(s, "ξ: abort() without an active value");
      s = nullptr; }

  void commit(uN size = 0)
    { A(s,          "ξ: commit(" << size << ") without an active value");
      A(size <= *s, "ξ: commit(" << size << ") larger than reservation of " << *s);
      if (size) z.advance((*s = size) + uNs);
      else      z.advance(*s + uNs);
      wt += *s + uNs;
      rg.w(true);  // we now have readable data
      s = nullptr; }


  // Reader interface: delete ξ to finalize the read-side, iterator and
  // unary */next() to read values
  Sn<u8> operator*() const
    { A(ra(), "*ξ with !ra(), " << z);
      return Sn<u8>(z + uNs, next_size()); }

  void next()
    { A(ra(), "++ξ with !ra(), " << z);
      z.free(next_size() + uNs);
      wg.w(true); }  // we are now writable


  struct it
  {
    ξ *const y;  // null == EOF

    bool          eof()            const { return !y || y->eof(); }
    bool   operator==(it const &x) const { return eof() == x.eof(); }
    it    &operator++()       { A(y, "++ξ::end"); y->next(); return *this; }
    Sn<u8> operator* () const { A(y,  "*ξ::end");            return **y; }
  };

  it begin() { return {this}; }
  it end()   { return {nullptr}; }


  // Endpoint management
  // NOTE: oq() conditionally weakens the input reference for reasons
  // outlined in doc/xi.md (ψ GC section)
  //
  // NOTE: iq() refers to input _to this ξ_ (i.e. the producer)
  //       oq() to output _from this ξ_ (i.e. the consumer)
  ξ &iq(Sp<ψ_> x) { iqs = x; if (w)    weaken(); return *this; }
  ξ &oq(Sp<ψ_> x) { oqw = x; if (!iqs) weaken(); return *this; }

  Sp<ψ_> iq() { return iqs ? iqs : iqw.lock(); }
  Sp<ψ_> oq() { return             oqw.lock(); }

  // Weaken reference to generating ψ, e.g. for backflowing ξ
  // NOTE: weaken() is, and must be, idempotent
  ξ &weaken() { if (!w) iqw = iqs, iqs.reset(), w = true; return *this; }


protected:
  ζ        z;
  u64      wt;   // total bytes written
  uN      *s;    // pointer to current element size
  bool     wc;   // writer has closed; all blocking reads will fail
  bool     w;    // weaken() has been called, always weaken source ψ
  λg<bool> rg;   // read-gate; false means insta-bail
  λg<bool> wg;   // write-gate; false means insta-bail
  Sp<ψ_>   iqs;  // input (supplying) ψ -- default strong reference
  Wp<ψ_>   iqw;  // input ψ -- optional weak reference
  Wp<ψ_>   oqw;  // output (consuming) ψ


  uN next_size() const
    { A(ra(), "pipe_next_size() with !ra(), " << z);
      return *Rc<uN*>(z + 0); }

  Sn<u8> reserve(uN l, bool nb)
    { for (;;)
        if      (let r = z.iptr(l + uNs)) { *(s = Rc<uN*>(r)) = l; return {r + uNs, l}; }
        else if (nb || !wg.y(λs::ξW))                              return {Sc<u8*>(nullptr), 0}; }


  friend O &operator<<(O&, ξ const&);
};


O &operator<<(O &s, ξ const &y);


}

#include "end.hh"


#endif
