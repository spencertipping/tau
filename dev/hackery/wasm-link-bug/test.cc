#include "test.hh"

thing_doer do_a_thing()
{
  return [](foo_type &x) {
    return std::visit(fn {
        [](auto &&x) -> foo_type { return 5; },
        [](int x) -> foo_type {
          std::shared_ptr<std::vector<int>> r{new std::vector<int>};
          for (int i = 0; i < x; ++i)
            r->push_back(i);
          return r; }
      }, x); };
}
