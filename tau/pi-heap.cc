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


}

#include "end.hh"
