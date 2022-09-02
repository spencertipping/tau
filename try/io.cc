#include "../tau.hh"

#if tau_defines_γ


#include <cstring>

#include "io.hh"

#include "../tau/begin.hh"


int cat(int argc, char **argv)
{
  Λ      l;
  Φ      f{l};
  fd_in  i{f, 0};
  fd_out o{f, 1};

  i.g | o.g;
  f.go();

  return 0;
}


int par(int argc, char **argv)
{
  Λ l;
  Φ f{l};

  fd_in  i{f, 0};
  delay  d{f, 100us};
  fd_out o{f, 1};
  i.g | d.g | o.g;

  iota       n{f};
  delay      d2{f, 10ms};
  stream_out o2{f, std::cerr};
  n.g | d2.g | o2.g;

  f.go();
  return 0;
}


int main(int argc, char **argv)
{
  if (!strcmp(argv[1], "cat")) return cat(argc - 2, argv + 2);
  if (!strcmp(argv[1], "par")) return par(argc - 2, argv + 2);
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
