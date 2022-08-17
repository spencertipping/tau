#include <cstring>
#include <iostream>
#include <vector>

#include <unistd.h>


#ifndef tau_debug
# define tau_debug 1
#endif


#include "utf9-basics.hh"
#include "util-basics.hh"

#if tau_defines_flux
# include "coro-basics.hh"
# include "psi-basics.hh"
# include "gamma-basics.hh"
# include "muon-basics.hh"
#endif


#include "../tau/module/begin.hh"


typedef int(*main_fn)();


int main(int argc, char **argv)
{
  using namespace std;

  if (argc == 1)
  {
    t::utf9_basics::main();
    t::util_basics::main();

#if tau_defines_flux
    t::coro_basics::main();
    t::psi_basics::main();
    t::gamma_basics::main();
    t::muon_basics::main();
#endif
  }
  else
  {
    vector<main_fn> fs;
    for (int i = 1; i < argc; ++i)
    {
      let a = argv[i];
      if      (!strcmp("utf9", a))  fs.push_back(t::utf9_basics::main);
      else if (!strcmp("util", a))  fs.push_back(t::util_basics::main);
#if tau_defines_flux
      else if (!strcmp("coro",  a)) fs.push_back(t::coro_basics::main);
      else if (!strcmp("psi",   a)) fs.push_back(t::psi_basics::main);
      else if (!strcmp("gamma", a)) fs.push_back(t::gamma_basics::main);
      else if (!strcmp("muon",  a)) fs.push_back(t::muon_basics::main);
      else if (!strcmp("muonb", a)) fs.push_back(t::muon_basics::bench);
#endif
      else
      {
        cout << "unknown argument: " << a << endl;
        _exit(1);
      }
    }

    for (let &f : fs) f();
  }

  return 0;
}


#include "../tau/module/end.hh"
