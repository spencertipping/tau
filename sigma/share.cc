#include "phi.hh"
#include "phi-str.hh"
#include "Gamma.hh"

#include "pi.hh"

#include "share.hh"
#include "begin.hh"

namespace τ
{


static void io_h(φd_<πfn>&);
static void loops_h(φd_<πfn>&);
static void pi_h(φd_<πfn>&);
static void routing_h(φd_<πfn>&);
static void serde_h(φd_<πfn>&);
static void strings_h(φd_<πfn>&);
static void topology_h(φd_<πfn>&);


void φshare(φd_<πfn> &f)
{
  io_h(f);
  loops_h(f);
  pi_h(f);
  routing_h(f);
  serde_h(f);
  strings_h(f);
  topology_h(f);
}


static void io_h(φd_<πfn> &f)
{
  f.def("»'", φnull(πpush(Γostream(std::cout))));
  f.def("Z", φinsn(φO(φatom(), πfn(πpush(3))), πf("ΓZ", ΓZ)));
}


static void loops_h(φd_<πfn> &f)
{
  f.def("▶", φinsn(φΓ1(), πf("▶", Γflex)));

  f.def("ι", φinsn(φO(φatom(), πfn(πpush(Nl<i64>::max()))),
                   πf("Γι", Γι)));

  f.def("ϊ", φnull(πpush(Γϊ())));

  f.def("↑", φinsn(φatom(), πf("Γ↑", Γtake)));
  f.def("↓", φinsn(φatom(), πf("Γ↓", Γdrop)));

  f.def("τ",  φnull(πpush(Γτ())));
  f.def("τT", φnull(πpush(Γτt(η0t::tuple))));
  f.def("τS", φnull(πpush(Γτt(η0t::set))));
  f.def("τM", φnull(πpush(Γτt(η0t::map))));

  f.def("Σi", φnull(πpush(Σi())));
  f.def("Σf", φnull(πpush(Σf())));
}


static void pi_h(φd_<πfn> &f)
{
  slet p = φπ();

  // π= one-shot execution
  f.def("π=", φinsn(φk(φq(p)), πf("π=", [](η x)
    { let f = *(*φE(p))(φc_(x.st())).y;
      return Γπ(f, x.st()); })));

  // π loop over inputs
  f.def("π*", φinsn(φk(φq(p)), πf("π*", [](η x)
    { let f = *(*φE(p))(φc_(x.st())).y;
      πfn b = f.q();
      b << πinsn("ξi*", [](πi &i)
        { let fn = i.dpop().as_η().pu();
          for (let x : i.i()) i.dclear().dpush(x).run(fn); });
      return Γπ(b, x.st()); })));

  // π 1:1 map
  f.def("π:", φinsn(φk(φq(p)), πf("π:", [](η x)
    { let f = *(*φE(p))(φc_(x.st())).y;
      πfn b = f.q();
      b << πinsn("ξi:", F<πinsn_ret(πi&)>([](πi &i)
        { let fn = i.dpop().as_η().pu();
          for (let x : i.i())
            if (!x.ts())
            { i.dclear().dpush(x).run(fn);
              if (!(i.o() << i.dpop().as_η())) return πinsn_error; }
          return πinsn_ok;
        }));
        return Γπ(b, x.st()); })));

  // π 1:1 map with τ after each item
  f.def("π;", φinsn(φk(φq(p)), πf("π;", [](η x)
    { let f = *(*φE(p))(φc_(x.st())).y;
      πfn b = f.q();
      b << πinsn("ξi;", F<πinsn_ret(πi&)>([](πi &i)
        { let fn = i.dpop().as_η().pu();
          for (let x : i.i())
            if (!x.ts())
            { i.dclear().dpush(x).run(fn);
              if (!(i.o() << i.dpop().as_η())
                  || !(i.o() << η1o(η1sig::τ)))
                return πinsn_error; }
          return πinsn_ok;
        }));
      return Γπ(b, x.st()); })));

  // π loop, appending to each input tuple
  f.def("π«", φinsn(φk(φq(p)), πf("π«", [](η x)
    { let f = *(*φE(p))(φc_(x.st())).y;
      πfn b = f.q();
      b << πinsn("ξi«", F<πinsn_ret(πi&)>([](πi &i)
        { let fn = i.dpop().as_η().pu();
          for (let x : i.i())
            if (x.tT())
            { i.dclear().dpush(x).run(fn);
              η0o o(x.t());
              for (let y : x.T()) o << y;
              o << i.dpop().as_η();
              if (!(i.o() << o)) return πinsn_error; }
            return πinsn_ok; }));
      return Γπ(b, x.st()); })));
}


static void routing_h(φd_<πfn> &f)
{
  f.def(":", φinsn(φπs(φword(), φΓ1()), {":", [](πi &i)
    { let g = i.dpop().as_Γ();
      let w = i.dpop().as_η().st();
      i.dpush(Γsub(w, g)); }}));
}


static void serde_h(φd_<πfn> &f)
{
  f.def("⍕", φnull(πpush(Γηbytes())));
  f.def("⍎", φnull(πpush(Γbytesη())));
}


static void strings_h(φd_<πfn> &f)
{
  f.def("N", φnull(πpush(Γsplit_chr("\n"))));
  f.def("T", φnull(πpush(Γtsv())));

  f.def("s", φinsn(φatom(), πf("Γs", Γsplit_chr)));
  f.def("S", φinsn(φatom(), πf("ΓS", Γsplit_str)));

  f.def("s⌈", φinsn(φπs(φatom(), φatom()), {"Γs⌈", [](πi &i)
    { let cs = i.dpop().as_η().st();
      let l  = i.dpop().as_η().pu();
      i.dpush(Γsplit_chr_bounded(cs.c_str(), l)); }}));

  f.def("S⌈", φinsn(φπs(φatom(), φatom()), {"ΓS⌈", [](πi &i)
    { let p = i.dpop().as_η().st();
      let l = i.dpop().as_η().pu();
      i.dpush(Γsplit_str_bounded(p, l)); }}));
}


static void topology_h(φd_<πfn> &f)
{
  f.def("i", φinsn(φatom(), πf("i", Γonce<η>)));
  f.def("⍝", φnull(πpush(Γcap("⍝"))));
}


}

#include "end.hh"
