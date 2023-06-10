#include <iomanip>

#include "pi-heap.hh"
#include "begin.hh"


#ifndef τπheap_print_all
# define τπheap_print_all 0
#endif


namespace τ
{


B &πh_b(πh &h) { return h.h(); }


void ηoc<πh&>::abort() { b.resize(s); s = 0; }
void ηoc<πh&>::commit(uN n)
{
  if (n)
  {
    b.resize(s + πhrns + n);
    h.ref({πhrn(s + πhrns), πhrn(n), 0, πhrn(n)});
  }
  s = 0;
}

Sn<u8> ηoc<πh&>::iptr(uN n)
{
  A(!s, "ηoc<πh&>::iptr(" << n << ") called with uncommitted data");
  h.reserve(n);
  b.resize((s = b.size()) + πhrns + n);
  *Rc<πhrn*>(b.data() + s) = 0;  // initialize relocation marker
  return {b.data() + s + πhrns, n};
}


void πh::reserve(uN l)
{
  if (h_.size() + l + πhrns > h_.capacity())
    gc(l + πhrns);
}


void πh::gc(uN l)
{
  A(!hn_, "πh::gc(" << l << ") called recursively");
  A(!ls_, "πh::gc(" << l << ") called with " << ls_ << " GC lock(s)");
  B hn;
  hn_ = &hn;
  s_  = 0;
  for (let v : vs_) v->mark();  // calculate live-set size
  let hns = std::max(s_ * 4, s_ + l + hr_);
  A(πhrn(hns) == hns,
    "πh heap size overflow: requested " << hns
    << " bytes, max is " << Nl<πhrn>::max());
  hn_->reserve(hns);
  h_.swap(hn);
  for (let v : vs_) v->move();  // move objects to new heap
  hn_ = nullptr;
  ++gs_;
}


void πh::mark(πhr const &r)
{
  A(hn_, "πh::mark(" << r << ") called outside of gc");
  s_ += r.l + πhrns;
}


πhr πh::move(πhr const &r)
{
  A(hn_, "πh::move(" << r << ") called outside of gc");
  if (let d = *Rc<πhrn*>(hn_->data() + r.o - πhrns))
    return {πhrn(d), r.l, r.i, r.il};

  let d = h_.size() + πhrns;
  h_.append(hn_->data() + r.o - πhrns, r.l + πhrns);
  *Rc<πhrn*>(hn_->data() + r.o - πhrns) = d;
  return {πhrn(d), r.l, r.i, r.il};
}


O &operator<<(O &s, πh const &h)
{
  s << "πh[s=" << h.h().size() << " c=" << h.h().capacity()
    << " lss=" << h.lss() << " hr=" << h.hr() << "]";

#if τπheap_print_all
  for (uN i = 0; i < h.h().size(); ++i)
  {
    if (i % 16 == 0) s << "\n ";
    s << " " << std::hex << std::setw(2) << std::setfill('0') << (int) h.h()[i];
  }
  return s << std::endl << std::dec << std::setw(0) << std::setfill(' ');
#else
  return s;
#endif
}


O &operator<<(O &s, πhr const &r)
{
  return s << r.o << "+" << r.l << ":" << r.i << "+" << r.il;
}


}

#include "end.hh"
