#include <functional>
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


int main()
{
  try_polymorphic_functions();
  try_variant_cast();
  return 0;
}


#include "../tau/end.hh"
