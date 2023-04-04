#include <iostream>
#include "../tau.hh"

using namespace τ;
using namespace std;

#include "../tau/begin.hh"


void try_polymorphic_functions()
{
  let f = fn {
    [](int x) { return x + 1; },
    [](St x)  { return "foo" + x; }
  };

  cout << "f(5) = " << f(5) << endl;
  cout << "f(\"bar\") = " << f("bar") << endl;
}


void try_variant_cast()
{
  Va<    int, double> a = 5.0;
  Va<St, int, double> b = vc(a);
  vi([](auto &&x) { cout << "a = " << x << endl; }, a);
  vi([](auto &&x) { cout << "b = " << x << endl; }, b);
}


int main()
{
  try_polymorphic_functions();
  try_variant_cast();
  return 0;
}


#include "../tau/end.hh"
