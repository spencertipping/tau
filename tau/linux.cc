#include <fcntl.h>
#include <unistd.h>

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
  f.def("e", φinsn(φatom(), πf("Γe", [](η x)
    { V<St> argv;
      for (let y : x.T()) argv.push_back(y.st());
      return Γfork_exec(argv, "err"); })));

  f.def("∷", φinsn(φΓ1(), πf("∷", Γτfork)));
}


static void io_h(φd_<πfn> &f)
{
  f.def("«", φinsn(φO(φatom(), πfn(πpush(0))), πf("«", [](η f)
    { return f.tsu() ? Γfr(open(f.st().c_str(), O_RDONLY)) : Γfr(f.pu()); })));

  f.def("»", φinsn(φO(φatom(), πfn(πpush(1))), πf("»", [](η f)
    { return f.tsu() ? Γfw(open(f.st().c_str(), O_WRONLY | O_CREAT, 0x600)) : Γfw(f.pu()); })));

  f.def("<",  φnull(πpush(Γfcat(false))));
  f.def("<τ", φnull(πpush(Γfcat(true))));
}


static void net_h(φd_<πfn> &f)
{
  // TODO
}


static void pkv_h(φd_<πfn> &f)
{
  f.def("<@", φinsn(φπs(φatom(), φatom()),
                    {"Γ<@", [](πi &i)
                      { let t = i.dpop().as_η().st();
                        let f = i.dpop().as_η().st();
                        i.dpush(Γpkv_get(f, t)); }}));

  f.def(">@", φinsn(φπs(φatom(), φatom()),
                    {"Γ>@", [](πi &i)
                      { let t = i.dpop().as_η().st();
                        let f = i.dpop().as_η().st();
                        i.dpush(Γpkv_set(f, t)); }}));
}

#else
void φlinux(φd_<πfn>&) {}
#endif


}

#include "end.hh"
