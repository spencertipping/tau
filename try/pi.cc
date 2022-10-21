#define τdebug 1
#define τπ0debug_bounds_checks 0
#define τπ0debug_gc_postcopy_verify 0
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


void try_data_stack_slow()
{
  π0h<2>   h{64, 256, 0};
  π0hds<2> s{h};

  for (u64 i = 0; i < 100000; ++i) s << o9(i);
  while (s.size() > 1)
  { let x = Sc<u64>(h[s[0]]) + Sc<u64>(h[s[1]]);
    s.drop(2);
    s << o9(x); }

  let t = h[s.pop()];
  A(Sc<u64>(t) == 4999950000, t << " ≠ 4999950000");
  cout << "slow 100k: " << h.gΘ << endl;
}


void try_data_stack_fast()
{
  π0h<2>   h{64, 65536, 0};
  π0hds<2> s{h};

  // Outer loop for better profiling if we want more data
  let ul = 10;
  for (uN l = 0; l < ul; ++l)
  { s << o9(Sc<u64>(0));
    for (u64 i = 0; i < 1ul << 24; ++i)
    { s << o9(i);
      s << o9(Sc<u64>(h[s.pop()]) + Sc<u64>(h[s.pop()])); }

    let t = h[s.pop()];
    A(Sc<u64>(t) == 140737479966720, t << " ≠ 140737479966720"); }

  cout << "fast " << 16 * ul << "M: " << h.gΘ << endl;
}


/*
void try_data_stack_tuple()
{
  π0h<2>   h{64, 65536, 0};
  π0hds<2> s{h};
  π0hnf<2> n{h, 2};

  V<uN> xs; xs.reserve(1ul << 24);
  for (uN i = 0; i < 1ul << 24; ++i) xs.push_back(i);

  auto &v = n << (h << o9(xs));
  auto &i = n << h(h[v].first().a);
  s << o9(Sc<uN>(0));

  while (i < h(h[v].next().a))
  { s << i;
    s << o9(Sc<uN>(h[s.pop()]) + Sc<uN>(h[s.pop()]));
    i = h(i.next().a); }

  let t = h[s.pop()];
  A(Sc<uN>(t) == 140737479966720, t << " ≠ 140737479966720");

  cout << "on-heap list: " << h.gΘ << endl;
}
*/


int main()
{
  try_simple_gc();
  try_data_stack_slow();
  try_data_stack_fast();
  //try_data_stack_tuple();
}


#include "../tau/end.hh"


#else

int main(int argc, char **argv)
{
  std::cout << "no π on this platform" << std::endl;
  return 0;
}

#endif
