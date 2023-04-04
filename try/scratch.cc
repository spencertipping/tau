#include <functional>
#include <iostream>
#include <type_traits>
#include "../tau.hh"

using namespace τ;
using namespace std;

#include "../tau/begin.hh"


void try_polymorphic_functions()
{
  // FIXME: we get fallthrough here for f("bar"), which is bad; we should
  // be able to specify a fallthrough branch with lower affinity (I hope)
  let f = fn {
    [](int x) { return x + 1; },
    [](St x)  { return "foo" + x; },
    [](auto x) { return "fallthrough"; }
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

  Va<St, int> c = "foo";
  vi(fn {
      [](St   x) { cout << "string branch: " << x << endl; },
      [](auto x) { cout << "fallthrough" << endl; } }, c);


  let f = [](int x) {};

  // FIXME: std::function_traits doesn't exist, but boost defines it
  // and https://stackoverflow.com/questions/25044869/member-function-traits.
  //using T = std::function_traits<decltype(&f::operator())>;

}


int main()
{
  try_polymorphic_functions();
  try_variant_cast();
  return 0;
}


#include "../tau/end.hh"
