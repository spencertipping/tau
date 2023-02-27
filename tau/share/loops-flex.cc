#include "loops.hh"
#include "../begin.hh"

namespace τ
{


struct Γflex_ : public virtual Γ_
{
  Γ g;
  Γflex_(Γ g_) : g(g_) {}

  St name() const { return "[‥" + g.name() + "‥]"; }

  void operator()(Ξ &x) const
    { Sp<ψ> sq = x.q("[‥");          // split prefix from data
      Sp<ψ> mq = x.q("‥]");          // merge prefix onto result
      let [pw, pr] = x.pipe(sq, mq);  // pipe for prefixes
      let [i1, o1] = x.xf(sq);

      sq->def([pw=pw, i1=i1, o1=o1]() mutable
        { for (let x : i1)
            if (x.tT())
            { let n = x.len();
              η0o p(η0t::tuple);
              uN i = 0;
              for (let y : x.T())
                if (++i < n) p << y;
                else if (!(o1 << y)) goto done;
              if (!(pw << p)) goto done; }
            else
              if (!(pw << η1o(η1sig::τ)) || !(o1 << x)) goto done;
        done:
          o1.close();
          i1.close();
          pw.close(); });

      g(x);

      let [i2, o2] = x.xf(mq);
      mq->def([pr=pr, i2=i2, o2=o2]() mutable
        { for (let p : pr)
            if (p.τ())
            { for (let x : i2)
                if      (x.τ())    { ++i2; break; }
                else if (!(o2 << x)) goto done; }
            else
              for (let x : i2)
                if (x.τ()) { ++i2; break; }
                else
                { η0o o(η0t::tuple);
                  for (let q : p.T()) o << q;
                  o << x;
                  if (!(o2 << o)) goto done; }
        done:
          o2.close();
          i2.close();
          pr.close(); }); }
};


Γ Γflex(Γ g) { return new Γflex_(g); }


}

#include "../end.hh"
