#ifndef tau_muon_multiplex_h
#define tau_muon_multiplex_h


#include "../types.hh"
#include "../utf9.hh"
#include "../flux.hh"
#include "../util.hh"


#include "../module/begin.hh"

namespace tau::muon
{


struct multiplex
{
  M<u9, u9>  m;
  γ         &g;

  // TODO: allow the user to specify which side(s) new mappings can come from
  multiplex(Γ &G) : g(G.γc())
    {
      g.ψw("io"y, α);
      g.ψw("default"y, α);
      g.ψw("ctrl"y, α);

      g.λc("demux"y, [&]() {
        if (!g.ψφw("io"y)) return 1;
        while (g.ψrw("io"y))
        {
          let v = g.ψr("io"y);
          if (!v.has_type(utf9::multi_types) || v.len() != 2 || !m.contains(v[0]))
            g.φe("default"y) && g.ψww("default"y) && g.ψw("default"y, v);
          else
          {
            let k = m.at(v[0]);
            if (!g.φe(m.at(v[0])) || !g.ψww(k) || g.ψw(k, v[1]))
              g.φe("default"y) && g.ψww("default"y) && g.ψw("default"y, v);
          }
        }

        return 0;
      });

      g.λc("ctrl"y, [&]() {
        while (g.ψφw("ctrl"y))
        {
          auto s = g.gs();
          g.ψm("ctrl"y, s).λc(s, [&, s = std::move(s)]() {
            if (!g.ψrw(s)) return 1;
            let k = g.ψr(s);

            if (m.contains(k)) return 1;
            m[k] = s;
            while (g.ψrw(s))
              if (g.ψww("io"y)) g.ψw("io"y, u9t() << k << g.ψr(s));

            g.ψw(m.at(k), ω);
            m.erase(k);
            return 0;
          });
        }
        return 0;
      });
    }

  ~multiplex() { g.g.γx(g.i); }
};


}

#include "../module/end.hh"


#endif
