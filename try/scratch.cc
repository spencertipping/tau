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


void try_pi_bound_fns()
{
  τe t;
  πi i(t);

  let x = [](πi&) { return πv{3}; };
  let y = [](πi&) { return πv{4}; };
  let add1 = fn {
    [](πi&, auto &&x, auto &&y) -> πv { cout << "type error" << endl; return {0}; },
    [](πi&, i64    x, i64    y) -> πv { return {x + y}; } };

  let add1_x_y = πde(add1, x, y);
  std::visit(fn {
      [](auto &&x) { cout << "type error on add1_x_y" << endl; },
      [](i64    x) { cout << "[add1] x + y = " << x << endl; }},
    add1_x_y(i).v_);

  let add2_x_y = πdc(πηadd(), x, y);
  std::visit(fn {
      [](auto &&x) { cout << "type error on add2_x_y" << endl; },
      [](i64    x) { cout << "[add2] x + y = " << x << endl; }},
    add2_x_y(i).v_);
}


void try_pi_lazy_fns()
{
  τe t;
  πi i(t);

  let w = [](πi&) -> πv { return πv{0}; };
  let x = [](πi&) -> πv { return πv{1}; };
  let y = [](πi&) -> πv { throw "should never happen"; };
  let z = [](πi&) -> πv { return πv{3}; };
  let or_fn = fn {
    [](πi &i, auto &&x, πfc &r) -> πv { cout << "type error" << endl; throw "type error"; },
    [](πi &i, i64    x, πfc &r) -> πv { return x ? πv{x} : r(i); } };

  let or_x_y = πdl(or_fn, x, y);
  std::visit(fn {
      [](auto &&x) { cout << "type error on or_x_y" << endl; },
      [](i64    x) { cout << "1 || error = " << x << endl; }},
    or_x_y(i).v_);

  let or_w_z = πdl(or_fn, w, z);
  std::visit(fn {
      [](auto &&x) { cout << "type error on or_w_z" << endl; },
      [](i64    x) { cout << "0 || 3 = " << x << endl; }},
    or_w_z(i).v_);
}


int main()
{
  try_polymorphic_functions();
  try_variant_cast();
  try_pi_bound_fns();
  try_pi_lazy_fns();
  return 0;
}


#include "../tau/end.hh"
