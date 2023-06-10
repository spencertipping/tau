#ifndef τπheap_h
#define τπheap_h

#include "xiio.hh"
#include "begin.hh"

namespace τ
{


struct πhr;
struct πh;
struct πhv;


// Reference to π heap value
struct πhr final
{
  uN o;  // offset relative to start of heap
  uN l;  // length in bytes
  uN i;  // inner offset
};


B &πh_b(πh &h);  // NOTE: required to break circular dependency


// Writer for π heap value: informs the heap when a value is committed,
// and calls gc() for oversized values.
template<> struct ηoc<πh&> final
{
  ηoc(πh &h_) : h(h_), b(πh_b(h_)), s(0) {}

  bool   expired() { return false; }
  void   abort  ();
  void   commit (uN n);
  Sn<u8> iptr   (uN n);

protected:
  πh &h;
  B  &b;
  uN  s;  // last committed size
};


// π heap of linearly-allocated η values with GC. Each value is referred to
// by a πhr, which is a byte offset and length relative to the start of the
// current heap data. The heap is compacted during GC, so πhrs must be
// updated. This is done using heap views, which are notified when the heap
// is GC'd and can then update their πhrs.
//
// The GC algorithm is designed for simple, atomic C++ functions which
// receive η-packed values and return things which can be written out using
// operator<<. This is generally safe because operator<< accepts a materialized
// result.
//
// πhrs can contain one another, but we collect only toplevel references. This
// means that inner objects hold references to their parents even if the other
// parent data is not referred to.
struct πh final
{
  πh(uN hr = 1048576) : r_{0, 0, 0}, s_(0), hr_(hr), hn_(nullptr) { h_.reserve(hr); }

  // Read a value from the heap. Note that the result is not auto-updated
  // during GC, so you'll need to re-create the ηi if a GC may have happened.
  ηi operator[](πhr const &r) const
    { return ηi{h_.data() + r.o + r.i, r.l - r.i}; }

  // Refer to a ηi already on the heap and contained within a heap ref.
  πhr i(πhr const &r, ηi y) const
    { return {r.o, r.l, uN(y.odata() - (h_.data() + r.o))}; }

  // Write a value into the heap and return a reference to it.
  Tt πhr operator<<(T const &x)
    { A(!r_.l, "πh<< is not re-entrant");
      A(!hn_,  "πh<< during GC");
      ηo<πh&>{ηoc<πh&>{*this}, ηauto_<T>::n} << x;  // calls .ref()
      let r = r_; r_ = {0, 0, 0};
      return r; }

  // Called during operator<< to set the πhr of the value being written.
  // We don't always know this up front because GC can happen during a write,
  // which will rearrange the heap and change the result.
  void ref(πhr const &r) { r_ = r; }

  // GC with the specified amount of headroom for a new value that is going
  // to be written.
  void gc(uN);
  void reserve(uN);
  void mark(πhr const&);  // Mark a πhr as reachable. Called by πhv.
  πhr  move(πhr const&);  // Ask for the new location of a πhr. Called by πhv.

  void adv(πhv *v) { vs_.insert(v); }
  void rmv(πhv *v) { vs_.erase(v); }

  uN lss() const { return s_; }
  uN hr()  const { return hr_; }

  B &h() { return h_; }
  B const &h() const { return h_; }

protected:
  B       h_;   // current heap
  πhr     r_;   // last reference committed to heap
  uN      s_;   // live-set size
  uNc     hr_;  // headroom for new heap sizes
  B      *hn_;  // next heap (during GC, otherwise null)
  S<πhv*> vs_;  // heap views, which are notified when the heap is GCd
};


// A heap view, which allows the GC to update πhrs. πh calls mark() to
// indicate that the view should mark all of its references; then it calls
// move() to indicate that the view can ask for the new location of any
// of its marked references.
struct πhv
{
  πhv(πh &h_) : h(h_) { h.adv(this); }
  virtual ~πhv()      { h.rmv(this); }

  virtual void mark() = 0;
  virtual void move() = 0;

protected:
  πh &h;
};


// A stack-shaped view of a heap, with a stack of heap refs
struct πhsv : public virtual πhv
{
  πhsv(πh &h_) : πhv(h_) {}
  void mark() { for (let  &r : xs)     h.mark(r); }
  void move() { for (auto &r : xs) r = h.move(r); }
  V<πhr> xs;
};


// A heap view that manages C++ native variables
struct πhlv : public virtual πhv
{
  πhlv(πh &h_) : πhv(h_) {}
  void mark() { for (let  &r : xs)      h.mark(*r); }
  void move() { for (auto &r : xs) *r = h.move(*r); }
  πhlv &operator<<(πhr &r) { xs.push_back(&r); return *this; }
  V<πhr*> xs;
};


// A heap view that manages a string map of heap refs
struct πhmv : public virtual πhv
{
  πhmv(πh &h_) : πhv(h_) {}
  void mark() { for (let  &r : xs)            h.mark(r.second); }
  void move() { for (auto &r : xs) r.second = h.move(r.second); }
  M<St, πhr> xs;
};


O &operator<<(O&, πh const&);
O &operator<<(O&, πhr const&);


}

#include "end.hh"

#endif
