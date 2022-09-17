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
  for (uN i = 0; i < 10000; ++i)
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

  for (uN i = 0; i < 10000; ++i) s << i;
  while (s.depth() > 1)
  { let a = s[0];
    let b = s[1];
    s.pop(2);
    s << Sc<i64>(a) + Sc<i64>(b); }
  cout << s << endl;
}


int main(int argc, char **argv)
{
  try_symbols();
  try_gc_heap();
  try_gc_stack();
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
