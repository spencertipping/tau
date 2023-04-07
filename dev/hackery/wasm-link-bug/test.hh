#include <functional>
#include <variant>
#include <memory>
#include <vector>

typedef std::variant<
  std::shared_ptr<std::vector<int>>,
  std::shared_ptr<std::vector<double>>,
  int,
  double> foo_type;

typedef std::function<foo_type(foo_type&)> thing_doer;

template<class... Xs> struct fn : Xs... { using Xs::operator()...; };
template<class... Xs> fn(Xs...) -> fn<Xs...>;

thing_doer do_a_thing();
