#define τdebug 1
#define τπ0debug_bounds_checks 0
#define τdebug_i9st 1

#include "../tau.hh"

#if τdefines_π


#if τπ0debug_specialize_templates
# error π₀ templates should not be specialized here
#endif


using namespace τ;
using namespace std;


#include "../tau/debug.hh"
#include "../tau/begin.hh"


void try_simple_gc()
{
  π0h<2> h{64, 256, 0};
  π0hnf<2> f{h, 4};
  auto &a = f << (h << o9t(1, 2, 3));
  auto &b = f << (h << o9t("foo", a, "bar"));
  auto &c = f << a;

  a = h << o9("new value for a");

  cout << a << " = " << h[a] << endl;
  cout << b << " = " << h[b] << endl;
  cout << c << " = " << h[c] << endl;
  cout << *h.hs[0] << endl;

  h.gc();
  cout << a << " = " << h[a] << endl;
  cout << b << " = " << h[b] << endl;
  cout << c << " = " << h[c] << endl;
  cout << *h.hs[0] << endl;
}


int main()
{
  try_simple_gc();
}


#include "../tau/end.hh"


#else

int main(int argc, char **argv)
{
  std::cout << "no π on this platform" << std::endl;
  return 0;
}

#endif
