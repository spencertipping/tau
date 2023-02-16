#include "linux.hh"
#include "begin.hh"

namespace τ
{


#if τlinux

static void fork_h(φd_<πfn>&);
static void io_h(φd_<πfn>&);
static void net_h(φd_<πfn>&);
static void pkv_h(φd_<πfn>&);


void φlinux(φd_<πfn> &f)
{
  fork_h(f);
  io_h(f);
  net_h(f);
  pkv_h(f);
}


static void fork_h(φd_<πfn> &f)
{
  f.def("e", φinsn(φatom(), πf("γe", [](η x)
    { V<St> argv;
      for (let y : x.T()) argv.push_back(y.st());
      return γfork_exec(argv, "err"); })));

  f.def("∷", φinsn(φγ1(), πf("∷", γτfork)));
}


static void io_h(φd_<πfn> &f)
{
  f.def("«", φinsn(φO(φatom(), πfn(πpush(0))),
                   πf("«", [](η fd) { return γfr(fd.pu()); })));

  f.def("»", φinsn(φO(φatom(), πfn(πpush(0))),
                   πf("«", [](η fd) { return γfw(fd.pu()); })));

  f.def("<",  φnull(πpush(γfcat(false))));
  f.def("<τ", φnull(πpush(γfcat(true))));
}


static void net_h(φd_<πfn> &f)
{
  // TODO
}


static void pkv_h(φd_<πfn> &f)
{
  f.def("<@", φinsn(φπs(φatom(), φa<πfn>(φatom(), φword())),
                    {"γ<@", [](πi &i)
                      { let t = i.dpop().as_η().st();
                        let f = i.dpop().as_η().st();
                        i.dpush(γpkv_get(f, t)); }}));

  f.def(">@", φinsn(φπs(φatom(), φa<πfn>(φatom(), φword())),
                    {"γ>@", [](πi &i)
                      { let t = i.dpop().as_η().st();
                        let f = i.dpop().as_η().st();
                        i.dpush(γpkv_set(f, t)); }}));
}

#else
void φlinux(φd_<πfn>&) {}
#endif


}

#include "end.hh"
