#include "../tau.hh"

#if tau_defines_γ


#include <cstring>

#include "io.hh"

#include "../tau/begin.hh"


int cat(int argc, char **argv)
{
  Λ          l;
  Φ          f{l};
  fd_in      i{f, 0};
  fd_out     o{f, 1};
  stream_out e{f, cerr};

  i.g | o.g;
  //i.g & e.g;
  while (1)
  {
    f();
    l.go();
    cerr << "loop: i = " << i.g << endl;
    cerr << "loop: o = " << o.g << endl;
    cerr << l << endl;
  }
  return 0;
}


int main(int argc, char **argv)
{
  if (!strcmp(argv[1], "cat")) return cat(argc - 2, argv + 2);
  return 0;
}


#include "../tau/end.hh"


#else

int main()
{
  std::cerr << "γ not defined" << std::endl;
  return 1;
}

#endif
