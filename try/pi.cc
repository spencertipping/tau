#define τdebug 1
#define τπ0debug_bounds_checks 1
#define τπ0debug_gc_postcopy_verify 1
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
  π0h<2>   h{64, 256, 0};
  π0hnf<2> f{h, 16};
  auto &a = f << (h << o9t(1, 2, 3));
  auto &b = f << (h << o9t("foo", a, a, "bar"));
  auto &c = f << a;
  auto &d = f << (h << o9t(true, false, b, a, b));

  a = h << o9("new value for a");

  cout << "pre-GC" << endl;
  cout << a << " = " << h[a] << endl;
  cout << b << " = " << h[b] << endl;
  cout << c << " = " << h[c] << endl;
  cout << d << " = " << h[d] << endl;
  //cout << *h.hs[0] << endl;

  h.gc();
  cout << "post-GC" << endl;
  cout << a << " = " << h[a] << endl;
  cout << b << " = " << h[b] << endl;
  cout << c << " = " << h[c] << endl;
  cout << d << " = " << h[d] << endl;
  //cout << *h.hs[0] << endl;

  // TODO: write assertions for this (one of the rare cases where
  // they're probably worth the time)
}


void try_data_stack()
{
  π0h<2>   h{64, 256, 0};
  π0hds<2> s{h};

  for (uN i = 0; i < 1000000; ++i) s << o9(i);
  while (s.size() > 1)
  { let x = Sc<uN>(h[s[0]]) + Sc<uN>(h[s[1]]);
    s.drop(2);
    s << o9(x); }

  let t = h[s.pop()];
  A(Sc<uN>(t) == 499999500000, t << " ≠ 499999500000");
  cout << h.gΘ << endl;
}


int main()
{
  try_simple_gc();
  try_data_stack();
}


#include "../tau/end.hh"


#else

int main(int argc, char **argv)
{
  std::cout << "no π on this platform" << std::endl;
  return 0;
}

#endif
