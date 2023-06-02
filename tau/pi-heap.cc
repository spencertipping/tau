#include "pi-heap.hh"
#include "begin.hh"

namespace τ
{


B   &ηoc<πh&>::b     ()     { return h.h(); }
void ηoc<πh&>::abort ()     {        b().resize(s);                    s = 0; }
void ηoc<πh&>::commit(uN n) { if (n) b().resize(s + n), h.ref({s, n}); s = 0; }

Sn<u8> ηoc<πh&>::iptr(uN n)
{
  A(!s, "ηoc<πh&>::iptr(" << n << ") called with uncommitted data");
  if (b().size() + n > b().capacity()) h.gc(n);
  b().resize((s = b().size()) + n);
  return {b().data() + s, n};
}


void πh::gc(uN l)
{
  A(!hn_, "πh::gc(" << l << ") called recursively");
  hn_ = new B;
  for (let v : vs_) v->mark();

  // TODO: calculate new heap size, reserve next heap

  for (let v : vs_) v->move();

  h_.swap(*hn_);
  delete hn_; hn_ = nullptr;
}


void πh::mark(πhr const &r)
{
  A(hn_, "πh::mark(" << r << ") called outside of gc");
  TODO("mark");
}


πhr πh::move(πhr const &r)
{
  A(hn_, "πh::move(" << r << ") called outside of gc");
  TODO("move");
  return r;
}


O &operator<<(O &s, πhr const &r)
{
  return s << r.o << "+" << r.l;
}


}

#include "end.hh"
