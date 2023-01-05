#define τdebug 1

//#define τdebug 1
//#define τdebug_iostream 1
//#define τallow_todo 1

//#define τπdebug_bounds_checks 1
//#define τπdebug_heapview_shuffle 1
//#define τdebug_i9st 1


#include <cstring>

#include "../tau.hh"


using namespace τ;
using namespace std;


#include "../tau/debug.hh"
#include "../tau/begin.hh"


// NOTE: this is legacy syntax; I should port it at some point
#define h(x) (x).deref()


void try_simple_gc()
{
  for (uN is = 0; is < 64; ++is)
  {
    πh   h{is, {256, 0}};
    πhnf f{h};
    i9 a = h << o9t(i8(1), i8(2), i8(3));
    i9 b = h << o9t("foo", πo9r(a), πo9r(a), "bar");
    i9 c = a;
    i9 d = h << o9t(true, false, πo9r(b), πo9r(a), πo9r(b));
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


void try_asm()
{
  πasm a{πabi2_linux()};
  a << "i32'3 [i32'4 :out] . : :out";

  Φ f;
  πint i{πabi2_linux(), a.build(), f, SP<πhgs>(new πhgs{f.ph})};
  i.run(0);
  i32 x = Sc<i32>(i.dpop());
  A(x == 3, "expected 3, got " << x);
}


void default_try_stuff()
{
  try_simple_gc();
  try_asm();
}


int main(int argc, char **argv)
{
  τassert_begin;
  default_try_stuff();
  τassert_end;
}


#include "../tau/end.hh"
