#include <algorithm>
#include <functional>
#include <ios>
#include <iostream>
#include <typeinfo>
#include <type_traits>
#include "../tau.hh"

using namespace τ;
using namespace std;

#include "../tau/begin.hh"


void try_polymorphic_functions()
{
  let f = fn {
    [](auto x) { return "fallthrough"; },
    [](int x)  { return x + 1; },
    [](chc *x) { return "foo" + St{x}; }
  };

  cout << "f(5) = " << f(5) << endl;
  cout << "f(\"bar\") = " << f("bar") << endl;
  cout << "f(true) = " << f(true) << endl;
}


void try_variant_cast()
{
  Va<    int, double> a = 5.0;
  Va<St, int, double> b = vc(a);
  std::visit([](auto &&x) { cout << "a = " << x << endl; }, a);
  std::visit([](auto &&x) { cout << "b = " << x << endl; }, b);

  std::visit([]<class T, class U>(T x, U y)
     { cout << "template invocation: " << x << ", " << y << endl; },
     a, b);

  Va<St, int> c = "foo";
  std::visit(fn {
      [](auto &&x) { cout << "fallthrough" << endl; },
      [](St     x) { cout << "string branch: " << x << endl; }}, c);
}


void try_auto()
{
  πf<-1> f = πvauto("test", [](i64 x, St y) -> St
    { return (Ss{} << "f(" << x << "," << y << ")").str(); });
  πi i;
  i.push(i << "foo");
  i.push(i << 5);
  f(i);
  cout << i[i.peek()] << endl;
}


void try_auto2()
{
  πf<0> f = πvauto("test", [](i64 x, St y) -> T<St, St>
                   { return {
                       (Ss{} << "f(" << x << "," << y << ")").str(),
                       "the second string"}; });
  πi i;
  i.push(i << "foo");
  i.push(i << 5);
  f(i);
  cout << i[i.pop()] << endl;
  cout << i[i.pop()] << endl;
}


void try_gc()
{
  sletc debug = false;
  πh   h;
  πhlv l{h};
  πhr  a = h << std::make_tuple(1, "foo", 3.14);
  πhr  b = h << std::make_tuple(2, "bar", 2.71, "WOOHOO");
  l << a << b;

  a = h << "baz bok";
  if (debug) cout << "h = " << h << "a = " << a << ", b = " << b << endl;
  if (debug) cout << "a = " << h[a] << ", b = " << h[b] << endl;
  A((Ss{} << h[a]).str() == "\"baz bok\"", "step 1: h[a] = " << h[a]);
  A((Ss{} << h[b]).str() == "2 \"bar\" 2.71 \"WOOHOO\"", "step 1: h[b] = " << h[b]);

  b = h.i(b, h[b].next());
  if (debug) cout << "h = " << h << "a = " << a << ", b = " << b << endl;
  if (debug) cout << "a = " << h[a] << ", b = " << h[b] << endl;
  A((Ss{} << h[a]).str() == "\"baz bok\"", "step 2: h[a] = " << h[a]);
  A((Ss{} << h[b]).str() == "\"bar\" 2.71 \"WOOHOO\"", "step 2: h[b] = " << h[b]);
  if (debug) cout << "GC" << endl;

  h.gc(0);

  if (debug) cout << "h = " << h << "a = " << a << ", b = " << b << endl;
  if (debug) cout << "a = " << h[a] << ", b = " << h[b] << endl;
  A((Ss{} << h[a]).str() == "\"baz bok\"", "step 3: h[a] = " << h[a]);
  A((Ss{} << h[b]).str() == "\"bar\" 2.71 \"WOOHOO\"", "step 3: h[b] = " << h[b]);
  cout << "GC OK" << endl;
}


void try_gc_auto()
{
  πf<-1> map_lookup = πvauto(
    "map_lookup", [](πi &i, πhr m, St k) -> πhr
      { πhgl l{i.h()};  // no GC is possible in this function
        let v = i[m][k];
        return i.i(m, v); });

  πf<-2> map_append = πvauto(
    "map_append", [](πi &i, πhr m, St k, πhr v) -> πhr
      { πhlv hv{i.h()};
        hv << m << v;
        let gcs = i.h().gcs();
        return i.r(i.h().hr() + 1,  // force a GC with a large reservation
                   [&](auto &&r)
                     { A(i.h().gcs() > gcs, "map_append didn't GC");
                       πhgl l{i.h()};
                       r << i[m].all();
                       r.k(k) << i[v].all(); }); });

  πi i;
  i.push(i.r(64, [](auto &&r) { r.k("foo") << "bar"; }));

  i.push(i.peek());              // m m
  i.push(i << "foo"); i.swap();  // m "foo" m
  map_lookup(i);
  cout << "m[foo] = " << i[i.pop()] << endl;

  i.push(i.peek());              // m m
  i.push(i << 13.5);  i.swap();  // m 13.5 m
  i.push(i << "bar"); i.swap();  // m "bar" 13.5 m
  map_append(i);
  cout << "m = " << i[i.peek()] << endl;

  i.push(i << "bar"); i.swap();  // m' "bar" m
  map_lookup(i);
  cout << "m'[bar] = " << i[i.pop()] << endl;

  i.push(i << "foo"); i.swap();  // m' "foo" m
  map_lookup(i);
  cout << "m'[foo] = " << i[i.pop()] << endl;

  cout << "GC auto OK" << endl;
}


int main()
{
  τassert_begin
  try_polymorphic_functions();
  try_variant_cast();
  try_auto();
  try_auto2();
  try_gc();
  try_gc_auto();
  return 0;
  τassert_end
}


#include "../tau/end.hh"
