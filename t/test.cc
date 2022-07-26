#include "utf9-basics.hh"
#include "coro-basics.hh"
#include "scheduler-basics.hh"


#include "../tau/module/begin.hh"

int main()
{
  t::utf9_basics::main();
  t::coro_basics::main();
  t::scheduler_basics::main();
  return 0;
}

#include "../tau/module/end.hh"
