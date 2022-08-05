#if (defined(__EMSCRIPTEN__) || defined(__LP64__))
#  define HAVE_CONTEXT 1
#else
#  define HAVE_CONTEXT 0
#endif


#include "utf9-basics.hh"

#if HAVE_CONTEXT
#  include "coro-basics.hh"
#  include "scheduler-basics.hh"
#endif


#include "../tau/module/begin.hh"


int main()
{
  t::utf9_basics::main();

#if HAVE_CONTEXT
  t::coro_basics::main();
  t::scheduler_basics::main();
#endif

  return 0;
}


#include "../tau/module/end.hh"
