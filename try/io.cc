#include <cstring>

#define tau_trace_Λ_switches 1
#include "io.hh"

#include "../tau/begin.hh"


int cat(int argc, char **argv)
{
  Λ          l;
  fd_in      i{l, 0};
  fd_out     o{l, 1};
  stream_out e{l, cerr};

  i.g | o.g;
  //i.g & e.g;
  l.go();
  return 0;
}


int main(int argc, char **argv)
{
  if (!strcmp(argv[1], "cat")) return cat(argc - 2, argv + 2);
  return 0;
}


#include "../tau/end.hh"
