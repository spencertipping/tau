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

  uN t = 0;
  for (uN i = 0; i < 1l << 24; ++i) t += i;

  A(Sc<i64>(s[0]) == t,
    "stack bench bogus sum: " << s[0] << " vs " << t);

  cout << s << endl;
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

  A(Sc<i64>(h[0]) == 1l << 24,
    "heap bench bogus sum: " << h[0] << " vs " << (1l << 24));

  cout << h << endl;
  cout << "16M numbers added via heap in "
       << t2 - t1 << endl;
}


void int_bench()
{
  πFs.clear();
  πfs.clear();

  πFs.push_back(new πF("i32", 1, [](πi &i, uN o) -> πr {
    i.d << Sc<i32>(i.p.b[o + 1]);
    i.n = o + 2;
    return 0;
  }));

  πFs.push_back(new πF("i32+", 0, [](πi &i, uN o) -> πr {
    i64 a = i.d[0];
    i64 b = i.d[1];
    i.d.pop(2);
    i.d << a + b;
    i.n = o + 1;
    return 0;
  }));

  πFs.push_back(new πF("get", 1, [](πi &i, uN o) -> πr {
    i.d << i.d[i.p.b[o + 1]];
    i.n = o + 2;
    return 0;
  }));

  πFs.push_back(new πF("j<", 1, [](πi &i, uN o) -> πr {
    i64 a = i.d[0];
    i64 b = i.d[1];
    i.d.pop(2);
    i.n = a < b ? i.p.b[o + 1] : o + 2;
    return 0;
  }));

  πFs.push_back(new πF("jmp", 1, [](πi &i, uN o) -> πr {
    i.n = i.p.b[o + 1];
    return 0;
  }));

  for (let f : πFs) πfs.push_back(f->f);


  πp p;
  p.ng = 0;
  p.p0 = 0;

  p.b.push_back(0);  // push constant i32
  p.b.push_back(0);  // constant = 0

  // loop start (bytecode offset = 2)
  p.b.push_back(2);  // stack-get
  p.b.push_back(0);  // idx = 0 (total)
  p.b.push_back(0);  // push i32
  p.b.push_back(1l << 24);
  p.b.push_back(3);  // jump if less
  p.b.push_back(πi::ni);

  p.b.push_back(0);  // i32 1
  p.b.push_back(1);

  p.b.push_back(1);  // scalar +

  p.b.push_back(4);  // jump to
  p.b.push_back(2);  // bytecode 2 (start of loop)


  Φ  f;
  πi i{f, p};

  for (uN j = 0; j < 100; ++j) i();
  cout << i << endl;

  let t1 = now();
  i.go();
  let t2 = now();

  cout << i << endl;
  cout << "bytecode sum 16M ints in " << t2 - t1 << endl;
}


int main(int argc, char **argv)
{
  try_symbols();
  try_gc_heap();
  try_gc_stack();
  gc_stack_bench();
  gc_heap_bench();
  int_bench();
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
