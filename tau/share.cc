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
  slet p = φπ();

  // π= one-shot execution
  f.def("π=", φinsn(φk(φq(p)), πf("π=", [](η x)
    { let f = *(*φE(p))(φc_(x.st())).y;
      return γπ(f, x.st()); })));

  // π loop over inputs
  f.def("π*", φinsn(φk(φq(p)), πf("π*", [](η x)
    { let f = *(*φE(p))(φc_(x.st())).y;
      πfn b = f.q();
      b << πinsn("ξi*", [](πi &i)
        { let fn = i.dpop().as_η().pu();
          for (let x : i.i()) i.dclear().dpush(x).run(fn); });
      return γπ(b, x.st()); })));

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
        return γπ(b, x.st()); })));

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
      return γπ(b, x.st()); })));

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
      return γπ(b, x.st()); })));
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
