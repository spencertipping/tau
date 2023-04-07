#include <iostream>
#include "test.hh"

using namespace std;

int main()
{
  foo_type x = 5;
  std::visit(fn {
      [](int a) { cout << "int branch" << endl; },
      [](double a) { cout << "double branch" << endl; },
      [](std::shared_ptr<std::vector<double>> a) {},
      [](std::shared_ptr<std::vector<int>> a) {
        for (auto y : *a) cout << y << endl;
      }
    }, do_a_thing()(x));
}
