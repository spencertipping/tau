#include "../tau.hh"
#include "../tau/begin.hh"

using namespace τ;
using namespace std;


int f(i64 x, St y)
{
  return x + y.size();
}


T<int, St> g(i64 x, St y)
{
  return std::make_tuple(x + 1, "a" + y);
}


void try_eta_auto1()
{
  ηm x;
  x << 1 << "foo" << 2 << "barbif";

  ηi i = x.y();

  let fa = ηauto(f);
  cout << "fa(i) = "    << fa(i) << endl;
  cout << "fa(i[2]) = " << fa(i[2]) << endl;
}


void try_eta_auto2()
{
  let ga = ηauto(g);
  ηm x; x << 1 << "foo" << 2 << "barbif";
  ηm y;
  y << ga(x.y());    cout << "ga(i) = "    << y << endl; y.clear();
  y << ga(x.y()[2]); cout << "ga(i[2]) = " << y << endl; y.clear();
}


int main()
{
  τassert_begin
  try_eta_auto1();
  try_eta_auto2();
  return 0;
  τassert_end
}

#include "../tau/end.hh"
