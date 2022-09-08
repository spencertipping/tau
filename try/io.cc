#define tau_debug_i9st 1

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
  delay  d{f, 10us};
  fd_out o{f, 1};
  i.g | d.g | o.g;

  iota       n{f, 10000};
  delay      d2{f, 1ms};
  stream_out o2{f, std::cerr};
  n.g | d2.g | o2.g;

  f.go();
  return 0;
}


int chat(int argc, char **argv)
{
  Λ l;
  Φ f{l};

  broadcast b{f};
  tcp_server s{f, 3000};

  γ chat{f};
  chat.λc([&]() {
    for (let x : chat)
    {
      auto fi = new fd_in {f, Sc<u9_pidfd>(x[0]).fd};
      auto fo = new fd_out{f, Sc<u9_pidfd>(x[0]).fd};
      auto sp = new splitter{f};

      std::cout << "chat got an item: " << x
                << "; fd = " << Sc<u9_pidfd>(x[0]).fd << std::endl;

      fi->g | sp->g | fo->g;
      b.g(b.p, sp->g, 2);
    }
    chat.ω();
    return 0;
  });

  s.g | chat;

  f.go();
  return 0;
}


int main(int argc, char **argv)
{
  if (!strcmp(argv[1], "cat"))  return cat (argc - 2, argv + 2);
  if (!strcmp(argv[1], "par"))  return par (argc - 2, argv + 2);
  if (!strcmp(argv[1], "chat")) return chat(argc - 2, argv + 2);
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
