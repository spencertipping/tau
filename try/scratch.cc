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


int main()
{
  try_polymorphic_functions();
  return 0;
}


#include "../tau/end.hh"
