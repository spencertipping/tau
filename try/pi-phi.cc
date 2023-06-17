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


void try_pi_phi()
{
  πφ f;
  f.def_sa(φm(πφint(),
              [](i64 x) { return πauto((Ss{} << x).str(),
                                       [x]() { return x; }); }));

  πi i;
  f.def_sp("+", [](       πpe<i64> x, i64 y) { return x.x + y; });
  f.def_sp(":", [](πi &i, πpe<π1>  x, i64 y) { x.x(i); return i[i.pop()].i() + y; });

  π1 f0 = φE(f.ts())("3").r();   f0(i); cout << "πφ:f0 → " << i[i.pop()] << endl;
  π1 f1 = φE(f.ts())("3+4").r(); f1(i); cout << "πφ:f1 → " << i[i.pop()] << endl;
  π1 f2 = φE(f.ts())("3:4").r(); f2(i); cout << "πφ:f2 → " << i[i.pop()] << endl;
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


int main()
{
  τassert_begin
  try_pi_phi();
  try_gc_auto();
  return 0;
  τassert_end
}


#include "../tau/end.hh"
