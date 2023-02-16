#include "phi.hh"
#include "phi-str.hh"
#include "gamma.hh"

#include "pi.hh"

#include "share.hh"
#include "begin.hh"

namespace τ
{


static void io_h(φd_<πfn>&);
static void loops_h(φd_<πfn>&);
static void pi_h(φd_<πfn>&);
static void routing_h(φd_<πfn>&);
static void strings_h(φd_<πfn>&);
static void topology_h(φd_<πfn>&);


void φshare(φd_<πfn> &f)
{
  io_h(f);
  loops_h(f);
  pi_h(f);
  routing_h(f);
  strings_h(f);
  topology_h(f);
}


static void io_h(φd_<πfn> &f)
{
  f.def("»'", φnull(πpush(γostream(std::cout))));
}


static void loops_h(φd_<πfn> &f)
{
  f.def("▶", φinsn(φγ1(), πf("▶", γflex)));

  f.def("ι", φinsn(φO(φatom(), πfn(πpush(Nl<i64>::max()))),
                   πf("γι", γι)));

  f.def("ϊ", φnull(πpush(γϊ())));

  f.def("↑", φinsn(φatom(), πf("γ↑", γtake)));
  f.def("↓", φinsn(φatom(), πf("γ↓", γdrop)));

  f.def("τ",  φnull(πpush(γτ())));
  f.def("τT", φnull(πpush(γτt(η0t::tuple))));
  f.def("τS", φnull(πpush(γτt(η0t::set))));
  f.def("τM", φnull(πpush(γτt(η0t::map))));
}


static void pi_h(φd_<πfn> &f)
{
  f.def("π", φinsn(φk(φq(φatom())), πf("π", [](η x)
    { return γπ(*(*φatom())(φc_(x.st())).y, x.st()); })));
}


static void routing_h(φd_<πfn> &f)
{
  f.def("%",  φinsn(φword(), πf("%", γswap)));
  f.def("⇐", φinsn(φγ1(), πf("γ⇐", γb)));

  f.def(":", φinsn(φπs(φword(), φγ1()), {":", [](πi &i)
    { let g = i.dpop().as_γ();
      let w = i.dpop().as_η().st();
      i.dpush(γsub(w, g)); }}));
}


static void strings_h(φd_<πfn> &f)
{
  f.def("N", φnull(πpush(γsplit_chr("\n"))));
  f.def("T", φnull(πpush(γtsv())));

  f.def("s", φinsn(φatom(), πf("γs", γsplit_chr)));
  f.def("S", φinsn(φatom(), πf("γS", γsplit_str)));

  f.def("s⌈", φinsn(φπs(φatom(), φatom()), {"γs⌈", [](πi &i)
    { let cs = i.dpop().as_η().st();
      let l  = i.dpop().as_η().pu();
      i.dpush(γsplit_chr_bounded(cs.c_str(), l)); }}));

  f.def("S⌈", φinsn(φπs(φatom(), φatom()), {"γS⌈", [](πi &i)
    { let p = i.dpop().as_η().st();
      let l = i.dpop().as_η().pu();
      i.dpush(γsplit_str_bounded(p, l)); }}));
}


static void topology_h(φd_<πfn> &f)
{
  f.def("i", φinsn(φatom(), πf("i", γonce<η>)));
}


}

#include "end.hh"
