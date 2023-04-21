#include "../tau.hh"
#include "../sigma.hh"

using namespace std;
using namespace τ;

#include "../tau/begin.hh"


i64 f(i64 x, St y)
{
  return x + y.size();
}


void try_auto()
{
  φ<i64> p = φE(φauto(f));
  A(p("3\"bar\"").r() == 6, "3\"bar\"");
  cout << "auto ok" << endl;
}


int main()
{
  try_auto();
  return 0;
}


#include "../tau/end.hh"
