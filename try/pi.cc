#define τdebug 1
//#define τπ0debug_bounds_checks 0
//#define τdebug_i9st 1

#include "../tau.hh"

#if τdefines_π


using namespace τ;
using namespace std;


#include "../tau/debug.hh"
#include "../tau/begin.hh"


int main()
{

}


#include "../tau/end.hh"


#else

int main(int argc, char **argv)
{
  std::cout << "no π on this platform" << std::endl;
  return 0;
}

#endif
