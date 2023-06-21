#include <algorithm>
#include <functional>
#include <ios>
#include <iostream>
#include <typeinfo>
#include <type_traits>

#include "../tau.hh"
#include "../tau/begin.hh"

using namespace τ;
using namespace std;


// Run a single π program with a minimal "standard library".
void pi(Stc &s)
{
  πφ0 f;
  f.def_sa(φm(πφint(),
              [](i64 x) { return πauto((Ss{} << x).str(),
                                       [x]() { return x; }); }));

  πi i;

  f.def_sa(φm(φl("FAIL"),
              [](St x) { return πauto("FAIL", []()
                { cerr << "FAIL" << endl;
                  return 0; }); }));

  f.def_sp("+", [](       πse<i64> x, i64 y) { return x.x + y; });
  f.def_sp("-", [](       πse<i64> x, i64 y) { return y - x.x; });
  f.def_sp(">", [](       πse<ηi>  x, ηi  y) { return x.x < y; });
  f.def_sp("<", [](       πse<ηi>  x, ηi  y) { return x.x > y; });
  f.def_sp("%", [](πi &i, πse<π1>  x, i64 y) { x.x(i); return i[i.pop()].i() + y; });
  f.def_sp("?", [](πi &i, πse<π1> x, φaL<':'>, πse<π1> y, i64 c)
    { c ? x.x(i) : y.x(i);
      return i.pop(); });

  f.def_sp("??", [](πi &i, πse<π1> x, πse<π1> y, i64 c)
    { c ? x.x(i) : y.x(i);
      return i.pop(); });

  // Functional-style while loop
  f.def_sp("!", [](πi &i, πst<π0> c, φaL<':'>, πst<π0> b, πhr x)
    { i.push(x);                              // stack = [x]
      while (1)
      { i.dup();                              // stack = [x x]
        c.x(i);                               // stack = [c(x) x]
        if (!i[i.pop()].b()) return i.pop();  // stack = [x]
        b.x(i); } });                         // stack = [x']

  cout << s << " → ";
  let t1 = now();
  π1 g = φE(f.ts())(s).r(); cout << g << " → ";
  let t2 = now();
  g(i);
  let t3 = now();
  cout << i[i.pop()];
  cout << " | parse: " << t2 - t1 << ", eval: " << t3 - t2 << endl;
}


void try_pi_phi()
{
  pi("FAIL");
  pi("3");
  pi("3+4");
  pi("3 + FAIL");
  pi("3%4");
  pi("3 % 4");
  pi("3 % FAIL");
  pi("1 ? 2 : 3");
  pi("0 ? 2 : 3");
  pi("1 ? 2 : FAIL");
  pi("0 ? FAIL : 3");
  pi("[1 - 2] ? 2 : 3");
  pi("[1 - 1] ? 2 : 3");
  pi("[1 - 2] ?? 2 3");
  pi("[1 - 1] ?? 2 3");
  pi("[1 - 2] ?? 2 FAIL");
  pi("[1 - 1] ?? FAIL 3");

  pi("10 ! >5 # condition\n : - 1 # body");

  // Uncomment to enable this benchmark case
  // pi("1000000 ! >1 # condition\n : - 1 # body");

  cout << "try_pi_phi OK" << endl;
}


void try_gc_auto()
{
  πf<-1> map_lookup = πauto(
    "m[]", [](πi &i, πhr m, St k) -> πhr
      { πhgl l{i.h()};  // no GC is possible in this function
        return i.i(m, i[m][k].one()); });

  πf<-2> map_append = πauto(
    "m++", [](πi &i, πhr m, St k, πhr v) -> πhr
      { πhlv hv{i.h()};
        hv << m << v;
        let gcs = i.h().gcs();
        return i.r(i.h().hr() + 1,  // force a GC with a large reservation
                   [&](auto &&r)
                     { A(i.h().gcs() > gcs, "map_append didn't GC");
                       πhgl l{i.h()};
                       r << i[m].all();
                       r.k(k) << i[v].all(); }); });

  πf<1> push_map1 = πauto(
    "{m}", [](πi &i) -> πhr
      { return i.r(64, [](auto &&r) { r.k("foo") << "bar"; }); });

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


int main(int argc, char **argv)
{
  τassert_begin
  try_pi_phi();
  try_gc_auto();
  return 0;
  τassert_end
}


#include "../tau/end.hh"
