#define τdebug 1

//#define τdebug 1
//#define τdebug_iostream 1
//#define τallow_todo 1

//#define τπ0debug_bounds_checks 1
//#define τπ0debug_heapview_shuffle 1
//#define τdebug_i9st 1


#include <cstring>

#include "../tau.hh"


using namespace τ;
using namespace std;


#include "../tau/debug.hh"
#include "../tau/begin.hh"


void try_simple_gc()
{
  for (uN is = 0; is < 64; ++is)
  {
    π0h   h{is, {256, 0}};
    π0hnf f{h};
    i9 a = h << o9t(i8(1), i8(2), i8(3));
    i9 b = h << o9t("foo", π0o9r(a), π0o9r(a), "bar");
    i9 c = a;
    i9 d = h << o9t(true, false, π0o9r(b), π0o9r(a), π0o9r(b));
    i9 e = d.first();

    f(&a, &b, &c, &d, &e);

    a = h << o9("new value for a");
    let la = a.size();

    A(!a.flagged(), "try/pi pre false flag on a");
    A( b.flagged(), "try/pi pre b should be flagged");
    A(!c.flagged(), "try/pi pre false flag on c");
    A( d.flagged(), "try/pi pre d should be flagged");
    A(!e.flagged(), "try/pi pre false flag on e");

    if (!is)
    {
      cout << "pre-GC heap size = " << h.hs[0]->h.size() << endl;
      cout << (void*) a.a << ": " << a << endl;
      cout << (void*) b.a << ": " << b << endl;
      cout << (void*) c.a << ": " << c << endl;
      cout << (void*) d.a << ": " << d << endl;
      cout << (void*) e.a << ": " << e << endl;
    }

    h.gc();


    if (!is)
    {
      A(!a.flagged(), "try/pi post false flag on a");
      A( b.flagged(), "try/pi post b should be flagged");
      A(!c.flagged(), "try/pi post false flag on c");
      A( d.flagged(), "try/pi post d should be flagged");
      A(!e.flagged(), "try/pi post false flag on e");

      cout << "post-GC heap size = " << h.hs[0]->h.size() << endl;
      cout << (void*) a.a << "+" << a.osize() << ": " << a << endl;
      cout << (void*) b.a << "+" << b.osize() << ": " << b << endl;
      cout << (void*) c.a << "+" << c.osize() << ": " << c << endl;
      cout << (void*) d.a << "+" << d.osize() << ": " << d << endl;
      cout << (void*) e.a << "+" << e.osize() << ": " << e << endl;
    }

    // One of the rare cases where it's worth asserting everything.

    A(h(a) == St{"new value for a"},   "try/pi a");
    A(h(h(b)[0]) == St{"foo"},         "try/pi b[0]");
    A(h(h(b)[1]) == h(c),              "try/pi b[1]");
    A(h(h(b)[2]) == h(c),              "try/pi b[2]");
    A(h(h(b)[3]) == St{"bar"},         "try/pi b[3]");
    A(h(h(c)[0]) == 1,                 "try/pi c[0]");
    A(h(h(c)[1]) == 2,                 "try/pi c[1]");
    A(h(h(c)[2]) == 3,                 "try/pi c[3]");
    A(h(h(d)[0]).at<bool>(0) == true,  "try/pi d[0]");
    A(h(h(d)[1]).at<bool>(0) == false, "try/pi d[1]");
    A(h(h(d)[2])             == h(b),  "try/pi d[2]");
    A(h(h(d)[3])             == h(c),  "try/pi d[3]");
    A(h(h(d)[4])             == h(b),  "try/pi d[4]");
    A(h(e).at<bool>(0) == true,        "try/pi e");

    if (!is)
    {
      // One or both of b's (1,2,3) entries must be a reference.
      A(h(b)[1].is_πref() || h(b)[2].is_πref(), "try/pi h[1] NR || h[2] NR");
      A(h(d)[2].is_πref() || h(d)[4].is_πref(), "try/pi d[2] NR || d[4] NR");

      let rs = 3 + sizeof(ζp);
      let elss =
        la+2 +     // a = "new value for a"
        2 +        // d = tuple(
        6 +        //   true, false,
        2 +        //   tuple(
        10 + rs +  //     "foo", "bar", ref,
        11 +       //     tuple(1,2,3)),
        2*rs;      //   ref, ref)

      A(h.hs[0]->h.size() == elss,
        "try/pi wrong live-set size: " << h.hs[0]->h.size() << " ≠ " << elss);
    }
  }
  cout << "simple GC OK" << endl;
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


void try_data_stack_tuple()
{
  π0h   h{64, {65536, 0}};
  π0hds s{h};
  π0hnf n{h};

  V<u32> xs; xs.reserve(1ul << 24);
  for (u32 i = 0; i < 1ul << 24; ++i) xs.push_back(i);

  i9 v = h << o9(xs);
  i9 i = v.first();
  i9 e;
  n(&v, &i, [&]() { e = v.next(); });

  A(v.type() == u9t::tuple, "v has wrong type = " << v.type());
  A(i.type() == u9t::u32,   "i has wrong type = " << i.type());
  A(e.a == v.next().a, "e is not v.next()");

  h.gc();
  A(v.type() == u9t::tuple, "v has changed type; new = " << v.type());
  A(i.type() == u9t::u32,   "i has changed type; new = " << i.type());
  A(e.a == v.next().a, "e is no longer v.next()");

  s << o9(Sc<u64>(0));
  h.gc();
  A(v.type() == u9t::tuple, "v has changed type");
  A(i.type() == u9t::u32,   "i has changed type");
  A(e.a == v.next().a, "e is no longer v.next()");

  uN its = 0;
  while (i.a < e.a)
  { s << i;
    let a = Sc<u32>(s.pop());
    let b = Sc<u64>(s.pop());
    s << o9(a + b);
    if (!(++its & (1ul << 23) - 1)) h.gc();
    ++i; }

  A(its == 1ul << 24, "too few loop iterations");
  A(h.gΘ.n > 3,       "too few GCs");

  let t = s.pop();
  A(Sc<u64>(t) == 140737479966720, t << " ≠ 140737479966720");

  cout << "on-heap list with artificial GCs: " << h.gΘ << endl;
}


void try_asm()
{
  π0asm a{π0abi1()};
  a << "i32'3 [i32'4 :out] . _ :out";

  Φ f;
  π0int i{π0abi1(), a.build(), f, SP<π0hgs>(new π0hgs{f.ph})};
  i.run(0);
  i32 x = Sc<i32>(i.dpop());
  A(x == 3, "expected 3, got " << x);
}


void default_try_stuff()
{
  try_simple_gc();
  try_data_stack_slow();
  try_data_stack_fast();
#if τplatform != τplatform_wasm
  try_data_stack_tuple();
#else
  cout << "try_data_stack_tuple() disabled on wasm due to high memory usage" << endl;
#endif
  try_asm();
}


int asmrun(char *src)
{
  Φ f;
  π0asm a{π0abi1()}; a << St{src};
  π0int i{π0abi1(), a.build(), f, SP<π0hgs>(new π0hgs(f.ph))};
  i.run(0);
  return 0;
}


int asmdebug(char *src)
{
  Φ f;
  π0asm a{π0abi1()}; a << St{src};
  π0int i{π0abi1(), a.build(), f, SP<π0hgs>(new π0hgs(f.ph))};

  cout << "input program:" << endl;
  cout << i.p << endl;
  i.rs.push_back(0);

  uN s = 0;
  while (!i.rs.empty())
  { cout << "step " << ++s << endl;
    i.step();
    cout << i << endl; }

  cout << "program ended; final state:" << endl;
  cout << i      << endl;
  cout << i.h.gΘ << endl;
  return 0;
}


int main(int argc, char **argv)
{
  τassert_begin;
  if (argc > 2)
    if      (!strcmp(argv[1], "run"))   return asmrun  (argv[2]);
    else if (!strcmp(argv[1], "debug")) return asmdebug(argv[2]);
  default_try_stuff();
  τassert_end;
}


#include "../tau/end.hh"
