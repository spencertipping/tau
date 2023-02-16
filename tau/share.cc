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
  f.def("»'", φl("", πfn{πinsn{"γ»'", [](πi &i)
    { i.dpush(γostream(std::cout)); return πinsn_ok; }}}));
}


static void loops_h(φd_<πfn> &f)
{
  // TODO: factor this "transform top stack value" pattern into something easier
  // probably two pieces:
  //
  // 1. φm<πfn, πfn> that adds a single insn
  // 2. a single insn that transforms the top value

  f.def("▶", φm<πfn, πfn>(φγ1(), [](πfn f)
    { return f << πinsn{"γ▶", [](πi &i)
      { i.dpush(γflex(i.dpop().as_γ()));
        return πinsn_ok; }}; }));

  f.def("ι", φm<Op<πfn>, πfn>(φo(φatom()), [](auto f)
    { πfn r;
      if (f) r += *f;
      else   r << πinsn{"const -1", [](πi &i)
        { i.dpush(η1o(Nl<uN>::max()));
          return πinsn_ok; }};
      return r << πinsn{"γι", [](πi &i)
        { i.dpush(γι(i.dpop().as_η().pu()));
          return πinsn_ok; }}; }));

  f.def("ϊ", φl("", πfn{πinsn{"γϊ", [](πi &i)
    { i.dpush(γϊ()); return πinsn_ok; }}}));

  f.def("↑", φm<πfn, πfn>(φatom(), [](πfn f)
    { return f << πinsn{"γ↑", [](πi &i)
      { i.dpush(γtake(i.dpop().as_η().pu()));
        return πinsn_ok; }}; }));

  f.def("↓", φm<πfn, πfn>(φatom(), [](πfn f)
    { return f << πinsn{"γ↓", [](πi &i)
      { i.dpush(γdrop(i.dpop().as_η().pu()));
        return πinsn_ok; }}; }));

  f.def("τ", φl("", πfn{πinsn{"γτ", [](πi &i)
    { i.dpush(γτ()); return πinsn_ok; }}}));


  f.def("τT", φl("", πfn{πinsn{"γτT", [](πi &i) { i.dpush(γτt(η0t::tuple)); return πinsn_ok; }}}));
  f.def("τS", φl("", πfn{πinsn{"γτS", [](πi &i) { i.dpush(γτt(η0t::set));   return πinsn_ok; }}}));
  f.def("τM", φl("", πfn{πinsn{"γτM", [](πi &i) { i.dpush(γτt(η0t::map));   return πinsn_ok; }}}));
}


static void routing_h(φd_<πfn> &f)
{
  f.def("⇐", φm<πfn, πfn>(φγ1(), [](πfn f)
    { return f << πinsn{"γ⇐", [](πi &i)
      { i.dpush(γb(i.dpop().as_γ()));
        return πinsn_ok; }}; }));
}


}

#include "end.hh"
