#include <functional>
#include <iostream>
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
  vi([](auto &&x) { cout << "a = " << x << endl; }, a);
  vi([](auto &&x) { cout << "b = " << x << endl; }, b);

  Va<St, int> c = "foo";
  vi(fn {
      [](auto x) { cout << "fallthrough" << endl; },
      [](St   x) { cout << "string branch: " << x << endl; }}, c);
}


void try_pi_vals()
{
  τe t;
  πi i{{}, {t.l(), {}}, {}};

  πv a{3};
  πv b{4};
  auto add = πbin(fn {
    [](let &x, let &y) -> πv { cout << "type error" << endl; return {0}; },
    [](i64  x, i64  y) -> πv { return {x + y}; }});

  πv c{add(i, a, b)};

  vi(fn {
      [](let &x) { cout << "type error" << endl; },
      [](i64  x) { cout << "got " << x << endl; }}, c.v);
}


int main()
{
  try_polymorphic_functions();
  try_variant_cast();
  try_pi_vals();
  return 0;
}


#include "../tau/end.hh"
