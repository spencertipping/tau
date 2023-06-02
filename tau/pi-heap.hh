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

  bool contains(πhr const &r) const
    { return r.o >= o && r.o + r.l <= o + l; }

  bool operator< (πhr const &r) const { return o < r.o; }
  bool operator==(πhr const &r) const { return o == r.o && l == r.l; }
};


// Writer for π heap value: informs the heap when a value is committed,
// and calls gc() for oversized values.
template<> struct ηoc<πh&> final
{
  ηoc(πh &h_) : h(h_), s(0) {}

  B     &b      ();
  bool   expired() { return false; }
  void   abort  ();
  void   commit (uN n);
  Sn<u8> iptr   (uN n);

protected:
  πh &h;
  uN  s;  // last committed size
};


// π heap of linearly-allocated η values with GC. Each value is referred to
// by a πhr, which is a byte offset and length relative to the start of the
// current heap data. The heap is compacted during GC, so πhrs must be
// updated. This is done using heap views, which are notified when the heap
// is GC'd and can then update their πhrs.
struct πh final
{
  πh(uN s = 65536) : gc_(false) { h_.reserve(s); }

  // Read a value from the heap. Note that the result is not auto-updated
  // during GC, so you'll need to re-create the ηi if a GC may have happened.
  ηi operator[](πhr r) const { return ηi{h_.data() + r.o, r.l}; }

  // Write a value into the heap and return a reference to it.
  Tt πhr operator<<(T const &x)
    { A(!r_.l, "πh<< is not re-entrant");
      ηo<πh&>{ηoc<πh&>{*this}, ηauto_<T>::n} << x;  // calls .ref()
      let r = r_; r_ = {0, 0};
      return r; }

  // Called during operator<< to set the πhr of the value being written.
  // We don't always know this up front because GC can happen during a write,
  // which will rearrange the heap and change the result.
  void ref(πhr r) { r_ = r; }

  // GC with the specified amount of headroom for a new value that is going
  // to be written.
  void gc(uN);
  void mark(πhr);  // Mark a πhr as reachable. Called by πhv.
  πhr  move(πhr);  // Ask for the new location of a πhr. Called by πhv.

  void adv(πhv *v) { vs_.insert(v); }
  void rmv(πhv *v) { vs_.erase(v); }

  B &h() { return h_; }

protected:
  B       h_;
  πhr     r_;
  S<πhv*> vs_;
  bool    gc_;  // is GC in progress?
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


}

#include "end.hh"

#endif
