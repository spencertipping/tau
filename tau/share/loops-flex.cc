#include "loops.hh"
#include "../begin.hh"

namespace τ
{


struct γflex_ : public virtual γ
{
  Sp<γ> g;
  γflex_(Sp<γ> g_) : g(g_) {}

  St name() const { return "[‥" + g->name() + "‥]"; }

  Ξ &operator()(Ξ &x)
    { Sp<ψ> sq = x.q("[‥");          // split prefix from data
      Sp<ψ> mq = x.q("‥]");          // merge prefix onto result
      let [pw, pr] = x.pipe(sq, mq);  // pipe for prefixes
      let [i1, o1] = x.xf(sq);

      sq->def([pw=pw, i1=i1, o1=o1]() mutable
        { for (let x : i1)
            if (x.type() == η0t::tuple)
            { let t = η1ti{x};
              let n = t.len();
              η0o p; p.t(η0t::tuple);
              uN i = 0;
              for (let y : t)
                if (++i < n) p << y;
                else if (!(o1 << y)) goto done;
              if (!(pw << p)) goto done; }
            else
              if (!(pw << η0o{η0sig::τ}) || !(o1 << x)) goto done;
        done:
          o1.close();
          i1.close();
          pw.close(); });

      (*g)(x);

      let [i2, o2] = x.xf(mq);
      mq->def([pr=pr, i2=i2, o2=o2]() mutable
        { for (let p : pr)
            if (η1i{p}.is_τ())
            { for (let x : i2)
                if (η1i{x}.is_τ()) { ++i2; break; }
                else if (!(o2 << x)) goto done; }
            else
              for (let x : i2)
                if (η1i{x}.is_τ()) { ++i2; break; }
                else
                { η0o o; o.t(η0t::tuple);
                  for (let q : η1ti{p}) o << q;
                  o << x;
                  if (!(o2 << o)) goto done; }
        done:
          o2.close();
          i2.close();
          pr.close(); });

      return x; }
};


Sp<γ> γflex(Sp<γ> g) { return Sp<γ>{new γflex_(g)}; }


}

#include "../end.hh"
