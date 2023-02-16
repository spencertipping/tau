#include "share.hh"
#include "begin.hh"

namespace τ
{


static void io_h(φd_<πfn>&);
static void loops_h(φd_<πfn>&);
static void routing_h(φd_<πfn>&);


void φshare(φd_<πfn> &f)
{
  io_h(f);
  loops_h(f);
  routing_h(f);
}


static void io_h(φd_<πfn> &f)
{
  f.def("»'", φnull(πpush(γostream(std::cout))));
}


static void loops_h(φd_<πfn> &f)
{
  f.def("▶", φinsn(φγ1(), πf("▶", γflex)));

  f.def("ι", φm<Op<πfn>, πfn>(φo(φatom()), [](auto f)
    { πfn r;
      if (f) r += *f;
      else   r << πpush(Nl<i64>::max());
      return r << πf("γι", γι); }));

  f.def("ϊ", φnull(πpush(γϊ())));

  f.def("↑", φinsn(φatom(), πf("γ↑", γtake)));
  f.def("↓", φinsn(φatom(), πf("γ↓", γdrop)));

  f.def("τ",  φnull(πpush(γτ())));
  f.def("τT", φnull(πpush(γτt(η0t::tuple))));
  f.def("τS", φnull(πpush(γτt(η0t::set))));
  f.def("τM", φnull(πpush(γτt(η0t::map))));
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


}

#include "end.hh"
