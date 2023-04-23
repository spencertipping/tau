#include "../tau.hh"
#include "../tau/begin.hh"

using namespace τ;
using namespace std;


int f(i64 x, St y)
{
  return x + y.size();
}


void try_eta_auto()
{
  ηm x;
  x << 1 << "foo" << 2 << "barbif";

  ηi i = x.y();

  let g = ηauto(f);
  cout << "g(i) = "    << g(i) << endl;
  cout << "g(i[2]) = " << g(i[2]) << endl;
}


int main()
{
  τassert_begin
  try_eta_auto();
  return 0;
  τassert_end
}

#include "../tau/end.hh"
