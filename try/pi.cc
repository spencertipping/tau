#define τdebug 1
//#define τπ0debug_bounds_checks 0
//#define τdebug_i9st 1

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
  π0h<2> h;
  let a = h << o9t(1, 2, 3);
  let b = h << o9t("foo", a, "bar");

  TODO("gc test");

  cout << a << endl;
  cout << b << endl;
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
