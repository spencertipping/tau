#include <iomanip>

#include "pi-heap.hh"
#include "begin.hh"

namespace τ
{


B &πh_b(πh &h) { return h.h(); }


void ηoc<πh&>::abort ()     {        b.resize(s);                                   s = 0; }
void ηoc<πh&>::commit(uN n) { if (n) b.resize(s + uNs + n), h.ref({s + uNs, n, 0}); s = 0; }

Sn<u8> ηoc<πh&>::iptr(uN n)
{
  A(!s, "ηoc<πh&>::iptr(" << n << ") called with uncommitted data");
  h.reserve(n);
  b.resize((s = b.size()) + uNs + n);
  *Rc<uN*>(b.data() + s) = 0;  // initialize relocation marker
  return {b.data() + s + uNs, n};
}


void πh::reserve(uN l)
{
  if (h_.size() + l + uNs > h_.capacity()) gc(l + uNs);
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
  s_ += r.l + uNs;
}


πhr πh::move(πhr const &r)
{
  A(hn_, "πh::move(" << r << ") called outside of gc");
  if (let d = *Rc<uN*>(hn_->data() + r.o - uNs)) return {d, r.l, r.i};

  let d = h_.size() + uNs;
  h_.append(hn_->data() + r.o - uNs, r.l + uNs);
  *Rc<uN*>(hn_->data() + r.o - uNs) = d;
  return {d, r.l, r.i};
}


O &operator<<(O &s, πh const &h)
{
  s << "πh[s=" << h.h().size() << " c=" << h.h().capacity()
    << " lss=" << h.lss() << " hr=" << h.hr() << "]";
  for (uN i = 0; i < h.h().size(); ++i)
  {
    if (i % 16 == 0) s << "\n ";
    s << " " << std::hex << std::setw(2) << std::setfill('0') << (int) h.h()[i];
  }
  return s << std::endl << std::dec << std::setw(0) << std::setfill(' ');
}


O &operator<<(O &s, πhr const &r)
{
  return s << r.o << "+" << r.l << ":" << r.i;
}


}

#include "end.hh"
