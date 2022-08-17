#include <thread>
#include <unistd.h>


#ifndef QUICK
# define QUICK 0
#endif


#include "../tau.hh"


#include "../tau/module/begin.hh"

namespace t::muon_basics
{

using namespace std;
using namespace tau;


void try_iota_cat()
{
  Γ g;
  let i = muon::iota(g, 10);
  let o = muon::ostream(g, cout);

  g.go();
}


int main()
{
  cout << "try_iota_cat" << endl; try_iota_cat();
  return 0;
}


}

#include "../tau/module/end.hh"
