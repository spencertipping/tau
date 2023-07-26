#include "multiplex.hh"
#include "begin.hh"

namespace σ
{


slet replicated_multiplex2_ = Ψauto([](Γ g, ψ q, ξi i, ξo o)
  { M<St, ξi> ls;
    M<St, ξo> rs;

    for (let x : i)
    {
      let k = St{x.one().s()};
      if (!ls.contains(k))
      {
        let [lo, li] = q.t().pipe();  // left side: we write into lo
        let [ro, ri] = q.t().pipe();  // right side: we read from ri
        ls[k] = ri;
        rs[k] = lo;
        g(Ξ{q.t()}.push({li, ro}));
        q.f([=, ri=ri]()
          { for (let x : ri) o.r(x.lsize() + 2 + k.size()) << k << x.all(); });
      }

      if (rs[k]) rs[k] << x;
      else       rs.erase(k), ls.erase(k);
    }
  });


slet static_multiplex2_ = Ψauto([](φrbrace<M<St, Γ>> gs, ψ q, ξi i, ξo o)
  { M<St, ξo> rs;

    for (let &[k, g] : gs.x)
    {
      let [lo, li] = q.t().pipe();
      let [ro, ri] = q.t().pipe();
      rs[k] = lo;
      g(Ξ{q.t()}.push({li, ro}));
      let g0 = g;
      A(ro.inner_ξ()->iq(), "{} Γ" << g0 << " failed to connect output (missing |?)");
      q.f([=, ri=ri, k=k]()
        { for (let x : ri) o.r(x.lsize() + 2 + k.size()) << k << x.all(); });
    }

    q.weaken(false);
    for (let x : i)
    {
      let k = St{x.one().s()};
      if (rs[k]) rs[k] << x.next();
    }
  });


slet static_multiplex4_ = Ψauto<true>(
  [](φrbrace<M<St, Γ>> gs, ψ q, ξi fi, ξo fo, ξo bo, ξi bi)
  { Sp<M<St, ξo>> frs{new M<St, ξo>};
    Sp<M<St, ξo>> brs{new M<St, ξo>};

    for (let &[k, g] : gs.x)
    {
      let [flo, fli] = q.t().pipe();
      let [blo, bli] = q.t().pipe();
      (*frs)[k] = flo;
      (*brs)[k] = blo;
      let x = g(Ξ{q.t()}.push({fli, blo}));
      let fri = x.f();
      let bri = x.b();
      q.f([=, fri=fri, k=k]()
        { for (let x : fri) fo.r(x.lsize() + 2 + k.size()) << k << x.all(); });
      q.f([=, bli=bli, k=k]()
        { for (let x : bli) bo.r(x.lsize() + 2 + k.size()) << k << x.all(); });
    }

    q.f([=]()
      { for (let x : fi)
        { let k = St{x.one().s()};
          if ((*frs)[k]) (*frs)[k] << x.next(); }});

    q.f([=]()
      { for (let x : bi)
        { let k = St{x.one().s()};
          if ((*brs)[k]) (*brs)[k] << x.next(); }});
  });


Γ replicated_multiplex2(Γ        g, Ψd d) { return ΓΨ(replicated_multiplex2_(g), d, "*[]"); }
Γ static_multiplex2    (M<St, Γ> g, Ψd d) { return ΓΨ(static_multiplex2_({g}),   d, "2{}"); }
Γ static_multiplex4    (M<St, Γ> g, Ψd d) { return ΓΨ(static_multiplex4_({g}),   d, "4{}"); }


void Γmultiplex(Γφ &g)
{
  g .def_p2("*",  Ψauto(replicated_multiplex2_))
    .def_p2("{",  Ψauto(static_multiplex2_))
    .def_p2("2{", Ψauto(static_multiplex2_))
    .def_c4("4{", Ψauto(static_multiplex4_))
    ;
}


}

#include "end.hh"
