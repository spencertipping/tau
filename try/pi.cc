#include <ostream>
#define tau_debug 1
//#define tau_debug_i9st 1
#include "../tau.hh"
#include "../tau/debug.hh"

#if τdefines_Φ


using namespace τ;
using namespace std;


#include "../tau/begin.hh"


void try_symbols()
{
  cout << u9_symbol::gensym() << std::endl
       << u9_symbol::gensym() << std::endl
       << u9_symbol::gensym() << std::endl;
}


void try_gc_heap()
{
  πh h(4);
  h.set(0, "A");
  h.set(1, "B");
  h.set(2, "C");
  h.set(3, "D");

  cout << h << endl;
  for (uN i = 0; i < 100000; ++i)
    h.set(3, i);
  h.clear(1);
  cout << h << endl;
}


void try_gc_stack()
{
  πs s;
  s << 1;
  s << 2;
  s << 3;
  s << s[0];
  while (s.depth() > 1)
  { let a = s[0];
    let b = s[1];
    s.pop(2);
    s << Sc<i64>(a) + Sc<i64>(b); }

  A(Sc<i64>(s[0]) == 9, "bogus stack sum: " << s[0] << " != 9");
  s.pop();

  for (uN i = 0; i < 100000; ++i) s << i;
  while (s.depth() > 1)
  { let a = s[0];
    let b = s[1];
    s.pop(2);
    s << Sc<i64>(a) + Sc<i64>(b); }
  cout << s << endl;
}


void gc_stack_bench()
{
  πs s;
  let t1 = now();
  for (uN i = 0; i < 1l << 24; ++i)
  { s << i;
    while (s.depth() > 1)
    { let a = s[0];
      let b = s[1];
      s.pop(2);
      s << Sc<i64>(a) + Sc<i64>(b); }}
  let t2 = now();

  cout << "16M numbers added via stack in "
       << t2 - t1 << endl;
}


void gc_heap_bench()
{
  πh h(8);
  let t1 = now();

  for (iN i = 0; i < 8; ++i)
    h.set(i, i);

  for (uN i = 0; i < 1l << 24; ++i)
  { let s = Sc<i64>(h[0]) + Sc<i64>(h[1]);
    h.set(0, s); }
  let t2 = now();

  cout << "16M numbers added via heap in "
       << t2 - t1 << endl;
}


int main(int argc, char **argv)
{
  try_symbols();
  try_gc_heap();
  try_gc_stack();
  gc_stack_bench();
  gc_heap_bench();
  return 0;
}

#include "../tau/end.hh"


#else

int main(int argc, char **argv)
{
  std::cout << "no π on this platform" << std::endl;
  return 0;
}

#endif
