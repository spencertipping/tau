#include "pi-heap.hh"
#include "begin.hh"

namespace τ
{


B &πh_b(πh &h) { return h.h(); }


void ηoc<πh&>::abort ()     {        b.resize(s);                          s = 0; }
void ηoc<πh&>::commit(uN n) { if (n) b.resize(s + uNs + n), h.ref({s, n}); s = 0; }

Sn<u8> ηoc<πh&>::iptr(uN n)
{
  A(!s, "ηoc<πh&>::iptr(" << n << ") called with uncommitted data");
  if (b.size() + n + uNs > b.capacity()) h.gc(n + uNs);
  b.resize((s = b.size()) + uNs + n);
  *Rc<uN*>(b.data() + s) = 0;  // initialize relocation marker
  return {b.data() + s + uNs, n};
}


void πh::gc(uN l)
{
  A(!hn_, "πh::gc(" << l << ") called recursively");
  B hn;
  hn_ = &hn;
  s_  = 0;
  for (let v : vs_) v->mark();  // calculate live-set size
  hn_->reserve(std::max(s_ * 4, s_ + l + hr_));
  h_.swap(hn);
  for (let v : vs_) v->move();  // move objects to new heap
  hn_ = nullptr;
}


void πh::mark(πhr const &r)
{
  A(hn_, "πh::mark(" << r << ") called outside of gc");
  s_ += r.l;
}


πhr πh::move(πhr const &r)
{
  A(hn_, "πh::move(" << r << ") called outside of gc");
  if (let d = *Rc<uN*>(hn_->data() + r.o)) return {d, r.l, r.i};

  let d = h_.size() + uNs;
  h_.append(hn_->data() + r.o - uNs, r.l + uNs);
  *Rc<uN*>(hn_->data() + r.o) = d;
  return {d, r.l, r.i};
}


O &operator<<(O &s, πhr const &r)
{
  return s << r.o << "+" << r.l;
}


}

#include "end.hh"
