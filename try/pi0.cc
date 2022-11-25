#define τdebug 1

//#define τdebug 1
//#define τdebug_iostream 1
//#define τallow_todo 1

//#define τπ0debug_bounds_checks 1
//#define τπ0debug_heapview_shuffle 1
//#define τdebug_i9st 1


#include <cstring>
#include <filesystem>
#include <iostream>

#include "../tau.hh"


using namespace τ;
using namespace std;
namespace fs = std::filesystem;


#include "../tau/begin.hh"


int asmrun(St const &src)
{
  // TODO: bind Φ here by creating a π₀
  π0h   h{};
  π0asm a{π0abi1()}; a << src;
  π0int i{π0abi1(), a.build(), h, SP<π0hgs>(new π0hgs{h})};
  i.run();
  return 0;
}


St input(char *x)
{
  fs::path p{x};
  if (!fs::exists(p)) return St{x};
  std::ostringstream s;
  std::filebuf b;
  if (b.open(x, std::ios::in)) { s << &b; b.close(); }
  return s.str();
}


int main(int argc, char **argv)
{
  τassert_begin;
  for (iN i = 1; i < argc; ++i) asmrun(input(argv[i]));
  τassert_end;
}


#include "../tau/end.hh"
