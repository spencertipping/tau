#ifndef tau_muon_multiplex_h
#define tau_muon_multiplex_h


#include "base.hh"
#include "../util.hh"


#include "../module/begin.hh"

namespace tau::muon
{


struct multiplex : base
{
  M<u9, γsi> m;

  // TODO: allow the user to specify which side(s) new mappings can come from
  multiplex(Γ &G_) : base(G_)
    {
      g.ψw("io"_q, α);
      g.ψw("default"_q, α);
      g.ψw("ctrl"_q, α);

      g.λc("demux"_l, [&]() {
        if (!g.ψφw("io"_q)) return 1;
        while (g.ψrw("io"_q))
        {
          let v = g.ψr("io"_q);
          if (!v.has_type(utf9::multi_types) || v.len() != 2 || !m.contains(v[0]))
            g.φe("default"_q) && g.ψww("default"_q) && g.ψw("default"_q, v);
          else
          {
            let k = m.at(v[0]);
            if (!g.φe(k) || !g.ψww(k) || g.ψw(k, v[1]))
              g.φe("default"_q) && g.ψww("default"_q) && g.ψw("default"_q, v);
          }
        }

        return 0;
      });

      g.λc("ctrl"_l, [&]() {
        while (g.ψφw("ctrl"_q))
        {
          auto s = g.gs();
          g.ψm("ctrl"_q, s).λc(s, [&, s = std::move(s)]() {
            if (!g.ψrw(s)) return 1;
            let k = g.ψr(s);

            if (m.contains(k)) return 1;
            m[k] = s;
            while (g.ψrw(s))
              if (g.ψww("io"_q)) g.ψw("io"_q, u9t() << k << g.ψr(s));

            g.ψw(m.at(k), ω);
            m.erase(k);
            return 0;
          });
        }
        return 0;
      });
    }
};


}

#include "../module/end.hh"


#endif
