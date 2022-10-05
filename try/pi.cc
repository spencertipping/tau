#define τdebug 1
//#define τπ0debug_bounds_checks 0
//#define τdebug_i9st 1

#include "../tau.hh"

#if τdefines_π


using namespace τ;
using namespace std;


#include "../tau/debug.hh"
#include "../tau/begin.hh"


void try_symbols()
{
  cout << u9_symbol::gensym() << endl
       << u9_symbol::gensym() << endl
       << u9_symbol::gensym() << endl;
}


void try_heap()
{
  π0h h;
  h.fpush(4);

  h.fs(0, "foo");
  h.fs(1, "bar");
  h.fs(2, o9t(1, 2, 3, "bar", true));
  h.fs(3, u9_symbol::gensym());

  cout << "h₀ = " << h.h.capacity() << ", " << h.gΘ << " " << h.gΘ.h << endl;
  for (uN i = 0; i < 100000; ++i) h.fs(0, o9t(i, "hi there", "woah", false));
  cout << "h₁ = " << h.h.capacity() << ", " << h.gΘ << " " << h.gΘ.h << endl;

  cout << h.fi(0) << " " << h.fi(1) << " " << h.fi(2) << " " << h.fi(3) << endl;
}


void try_stack()
{
  π0h h;

  let t0 = now();
  h.dpush(0);
  for (i64 i = 0; i < 1l << 24; ++i)
  { h.dpush(i);
    let a = Sc<i64>(h.di(0));
    let b = Sc<i64>(h.di(1));
    h.dpop(2);
    h.dpush(a + b); }
  let t1 = now();

  cout << "stack sum 16M ints = " << h.di(0) << " in " << t1 - t0 << " " << h.gΘ << endl;
}


void try_asm()
{
  bool got_7 = false;
  π0int i = π0asm()
    .def("i64+",  [](π0int &z, i9 a, i9 b) { z << Sc<i64>(a) + Sc<i64>(b); })
    .def("print", [&got_7](π0int &z, i9 a) { got_7 = a == 7; cout << a << endl; })
    .def("drop",  [](π0int &z, i9 a) {})
    .def(".",     [](π0int &z, i9 a) { z.r.push_back(a); })
    .begin()
    .l(1).l(2).f("i64+").f("print")
    .end()
    .f(".")
    .l(3).l(4)
    .f("i64+").f("print")
    .build();

  i.go();
  A(got_7, "we should have gotten 7; i = " << i);
}


int main()
{
  try_symbols();
  try_heap();
  try_stack();
  try_asm();
}


#include "../tau/end.hh"


#else

int main(int argc, char **argv)
{
  std::cout << "no π on this platform" << std::endl;
  return 0;
}

#endif
