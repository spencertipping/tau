#define τdebug 1
#define τπ0debug_bounds_checks 0
#define τπ0debug_gc_postcopy_verify 0
//#define τdebug_i9st 1


#include <cstring>

#include "../tau.hh"

#if τdefines_π


using namespace τ;
using namespace std;


#include "../tau/debug.hh"
#include "../tau/begin.hh"


void try_simple_gc()
{
  π0h   h{64, {256, 0}};
  π0hnf f{h};
  i9 a = h << o9t(1, 2, 3);
  i9 b = h << o9t("foo", π0o9r(a), π0o9r(a), "bar");
  i9 c = a;
  i9 d = h << o9t(true, false, π0o9r(b), π0o9r(a), π0o9r(b));

  f(a, b, c, d);

  a = h << o9("new value for a");

  cout << "pre-GC" << endl;
  cout << a << endl;
  cout << b << endl;
  cout << c << endl;
  cout << d << endl;

  h.gc();
  cout << "post-GC" << endl;
  cout << a << " = " << h(a) << endl;
  cout << b << " = " << h(b) << endl;
  cout << c << " = " << h(c) << endl;
  cout << d << " = " << h(d) << endl;

  // One of the rare cases where it's worth asserting everything.

  A(h(a) == St{"new value for a"}, "try/pi a");
  A(h(h(b)[0]) == St{"foo"}, "try/pi b[0]");
  A(h(h(b)[1]) == h(c),      "try/pi b[1]");
  A(h(h(b)[2]) == h(c),      "try/pi b[2]");
  A(h(h(b)[3]) == St{"bar"}, "try/pi b[3]");
  A(h(h(c)[0]) == 1,         "try/pi c[0]");
  A(h(h(c)[1]) == 2,         "try/pi c[1]");
  A(h(h(c)[2]) == 3,         "try/pi c[3]");
  A(h(h(d)[0]).at<bool>(0) == true,  "try/pi d[0]");
  A(h(h(d)[1]).at<bool>(0) == false, "try/pi d[1]");
  A(h(h(d)[2])             == h(b),  "try/pi d[2]");
  A(h(h(d)[3])             == h(c),  "try/pi d[3]");
  A(h(h(d)[4])             == h(b),  "try/pi d[4]");
}


void try_data_stack_slow()
{
  π0h   h{64, {256, 0}};
  π0hds s{h};

  for (u64 i = 0; i < 100000; ++i) s << o9(i);
  while (s.size() > 1)
  { let x = Sc<u64>(h(s[0])) + Sc<u64>(h(s[1]));
    s.drop(2);
    s << o9(x); }

  let t = h(s.pop());
  A(Sc<u64>(t) == 4999950000, t << " ≠ 4999950000");
  cout << "slow 100k: " << h.gΘ << endl;
}


void try_data_stack_fast()
{
  π0h   h{64, {65536, 0}};
  π0hds s{h};

  // Outer loop for better profiling if we want more data
  let ul = 1;
  for (uN l = 0; l < ul; ++l)
  { s << o9(Sc<u64>(0));
    for (u64 i = 0; i < 1ul << 24; ++i)
    { s << o9(i);
      s << o9(Sc<u64>(h(s.pop())) + Sc<u64>(h(s.pop()))); }

    let t = h(s.pop());
    A(Sc<u64>(t) == 140737479966720, t << " ≠ 140737479966720"); }

  cout << "fast " << 16 * ul << "M: " << h.gΘ << endl;
}


// FIXME: the function below does test something worthwhile, but
// I need to figure out the API for GC-safe tuple iteration. We
// probably need to have native frames that can hold i9 refs, not
// just π0r.

void try_data_stack_tuple()
{
  cout << "FIXME: try_data_stack_tuple()" << endl;
  /*
  π0h<2>   h{64, 65536, 0};
  π0hds<2> s{h};
  π0hnf<2> n{h, 2};

  V<uN> xs; xs.reserve(1ul << 24);
  for (uN i = 0; i < 1ul << 24; ++i) xs.push_back(i);

  // FIXME: this errors out in LSP, but not g++ or wasm builds
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
  */
}


/*
void try_asm()
{
  π0asm a{π0abi1()};
  a << "i32'3 [i32'4 :out] . _ :out";

  π0h   h{};
  π0int i{π0abi1(), a.build(), h};
  i.run(0);
  i32 x = i.dpop();
  A(x == 3, "expected 3, got " << x);
  } */


void default_try_stuff()
{
  try_simple_gc();
  try_data_stack_slow();
  try_data_stack_fast();
  try_data_stack_tuple();
  //try_asm();
}


/*
int asmrun(char *src)
{
  π0h   h{};
  π0asm a{π0abi1()}; a << St{src};
  π0int i{π0abi1(), a.build(), h};
  i.run(0);
  return 0;
}


int asmdebug(char *src)
{
  π0h   h{};
  π0asm a{π0abi1()}; a << St{src};
  π0int i{π0abi1(), a.build(), h};

  cout << "input program:" << endl;
  cout << i.p << endl;
  i.r.push_back(0);

  uN s = 0;
  while (!i.r.empty())
  { cout << "step " << ++s << endl;
    i.step();
    cout << i << endl; }

  cout << "program ended; final state:" << endl;
  cout << i      << endl;
  cout << i.h.gΘ << endl;
  return 0;
}
*/


int main(int argc, char **argv)
{
  τassert_begin;
  /*
  if (argc > 2)
    if      (!strcmp(argv[1], "run"))   return asmrun  (argv[2]);
    else if (!strcmp(argv[1], "debug")) return asmdebug(argv[2]);
  */
  default_try_stuff();
  τassert_end;
}


#include "../tau/end.hh"


#else

int main(int argc, char **argv)
{
  std::cout << "no π on this platform" << std::endl;
  return 0;
}

#endif
