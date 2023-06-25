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
  πf<-1> f = πauto("test", [](i64 x, St y) -> St
    { return (Ss{} << "f(" << x << "," << y << ")").str(); });
  πi i;
  i.push(i << "foo");
  i.push(i << 5);
  f(i);
  cout << i[i.peek()] << endl;
}


void try_auto2()
{
  πf<0> f = πauto("test", [](i64 x, St y) -> T<St, St>
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
  πf<-1> map_lookup = πauto(
    "m[]", [](πi &i, πhr m, ηname k) -> πhr
      { πhgl l{i.h()};  // no GC is possible in this function
        return i.i(m, i[m][k].one()); });

  πf<-2> map_append = πauto(
    "m++", [](πi &i, πhr m, ηname k, πhr v) -> πhr
      { πhlv hv{i.h()};
        hv << m << v;
        let gcs = i.h().gcs();
        return i.r(i.h().hr() + 1,  // force a GC with a large reservation
                   [&](auto &&r)
                     { A(i.h().gcs() > gcs, "map_append didn't GC");
                       πhgl l{i.h()};
                       r << i[m].all();
                       r << k << i[v].all(); }); });

  πf<1> push_map1 = πauto(
    "{m}", [](πi &i) -> πhr
      { return i.r(64, [](auto &&r) { r << ηname{"foo"} << "bar"; }); });

  πf<1>  push_foo  = πauto("'foo", []() { return "foo"; });
  πf<1>  push_bar  = πauto("'bar", []() { return "bar"; });
  πf<1>  push_13p5 = πauto("13.5", []() { return 13.5; });
  πf<0>  swap      = πauto("%",    [](πhr x, πhr y) { return std::make_tuple(y, x); });
  πf<1>  dup       = πauto(":",    [](πhr x)        { return std::make_tuple(x, x); });
  πf<-1> drop      = πauto("_",    [](πhr x)        { });

  πi i;

  let p1 = push_map1 | dup | push_foo | swap | map_lookup;
  cout << "m[foo]  via " << p1 << " = " << p1(i).ypop() << endl;

  let p2 = dup | push_13p5 | swap | push_bar | swap | map_append
    | swap | drop | dup;
  cout << "m'      via " << p2 << " = " << p2(i).ypop() << endl;

  let p3 = dup | push_foo | swap | map_lookup;
  cout << "m'[foo] via " << p3 << " = " << p3(i).ypop() << endl;

  let p4 = dup | push_bar | swap | map_lookup;
  cout << "m'[bar] via " << p4 << " = " << p4(i).ypop() << endl;

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
