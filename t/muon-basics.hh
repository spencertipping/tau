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
  let c = muon::cat(g, i, "out"_q, o, "in"_l);

  g.go();
}


int bench()
{
  struct sum : muon::base
  {
    sum(Γ &G_) : base(G_)
      { g.ψw("in"_q, α);
        g.ψw("out"_q, α);
        g.λc("main"_l, [&]() {
          u64 t = 0;
          while (g.ψrw("in"_q))
          { let v = g.ψr("in"_q);
            if (!v.is_greek()) t += Sc<u64>(v); }
          if (g.ψφw("out"_q) && g.ψww("out"_q)) g.ψw("out"_q, u9(t));
          else                                  cout << "no out???" << endl;
          return 0;
        }); }
  };


  Γ g;
  let i = muon::iota(g, QUICK ? 1ull << 16 : 1ull << 24);
  let s = sum(g);
  let o = muon::ostream(g, cout);

  let p1 = muon::cat(g, i, "out"_q, s, "in"_q);
  let p2 = muon::cat(g, s, "out"_q, o, "in"_q);

  Θp t1 = now();
  g.go();
  Θp t2 = now();

  cout << "sum " << i.n << " via muon: " << t2 - t1 << endl;
  cout << "  = " << (t2 - t1) / i.n << "/element" << endl;

  return 0;
}


int main()
{
  cout << "try_iota_cat" << endl; try_iota_cat();
  return 0;
}


}

#include "../tau/module/end.hh"
