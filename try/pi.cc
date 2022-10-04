#include <ostream>
#define tau_debug 1
//#define tau_debug_i9st 1
#include "../tau.hh"

#if τdefines_Φ


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

  cout << "h₀ = " << h.h.capacity() << ", " << h.gΘ << endl;
  for (uN i = 0; i < 100000; ++i) h.fs(0, o9t(i, "hi there", "woah", false));
  cout << "h₁ = " << h.h.capacity() << ", " << h.gΘ << endl;

  cout << h.fi(0) << " " << h.fi(1) << " " << h.fi(2) << " " << h.fi(3) << endl;
}


int main()
{
  try_symbols();
  try_heap();
}


#include "../tau/end.hh"


#else

int main(int argc, char **argv)
{
  std::cout << "no π on this platform" << std::endl;
  return 0;
}

#endif
