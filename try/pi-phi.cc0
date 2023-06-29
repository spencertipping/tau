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
  πφ<φlit, φbrack> f;
  f.def_sa(φm(φint_(),
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
  f.def_sp("!", [](πi &i, πse<π1> c, φaL<':'>, πse<π1> b, πhr x)
    { i.push(x);                            // stack = [x]
      i.def_x();
      while (1)
      { c.x(i);                             // stack = [c(x)]
        if (!i.ypop().b()) return i.x();
        b.x(i);                             // stack = [x']
        i.def_x(); } });

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


int main(int argc, char **argv)
{
  τassert_begin
  try_pi_phi();
  return 0;
  τassert_end
}


#include "../tau/end.hh"
