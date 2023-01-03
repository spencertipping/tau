#define τdebug 1

//#define τdebug 1
//#define τdebug_iostream 1
//#define τallow_todo 1

#define τπ0debug_bounds_checks 1
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


Φ f;

void run(π0asm &a)
{
  f.l.c([&]()
    { π0int(π0abi1(), a.build(), f, SP<π0hgs>(new π0hgs(f.ph))).run();
      return 0; });
  if (f.is_async()) f.go_async();
  else              f.go();
}


St input(char *x)
{
  if (strlen(x) > 255) return St{x};
  fs::path p{x};
  if (!fs::exists(p)) return St{x};
  std::ostringstream s;
  std::filebuf b;
  if (b.open(x, std::ios::in)) { s << &b; b.close(); }
  return s.str();
}


void repl(π0asm &a)
{
  St        l;
  SP<π0hgs> gs(new π0hgs(f.ph));
  π0int     i(π0abi1(), nullptr, f, gs);

  while (1)
  {
    cerr << "> " << flush;
    getline(cin, l);
    if (l.empty()) break;
    a << l;
    f.l.c([&]()
      { try
        {
          i.p = a.build_repl();
          i.run();
        }
        catch (Stc e)
        {
          cout << "! " << e << endl;
        }
        cout << i << endl;
        return 0; });
    if (f.is_async()) f.go_async();
    else              f.go();
    cout << f << endl;
  }
}


int main(int argc, char **argv)
{
  τassert_begin;
  π0asm a(π0abi1());
  iN   i = 1;
  bool r = false;
  if (argc > 1 && !strcmp(argv[1], "--repl")) r = true, ++i;
  for (; i < argc; ++i) a << input(argv[i]);
  r ? repl(a) : run(a);
  τassert_end;
  return 0;
}


#include "../tau/end.hh"
