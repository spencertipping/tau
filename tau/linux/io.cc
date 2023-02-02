#include "../arch.hh"
#if τplatform == τplatform_linux

#include "../psi.hh"
#include "../xiio.hh"
#include "../eta.hh"

#include "io.hh"
#include "../begin.hh"

namespace τ
{



static Sp<ψ> ψfd(τe &t, fd_t fd)
{
  return {new ψ(t), [=, &t](ψ*) { t.close(fd); }};
}


static Sp<ψ> ψr(Sp<ψ> q, fd_t fd, ξi i, ξo o)
{
  q->name((Ss{} << "<" << fd).str())
    .def([=](ψ &q) mutable
      { let s = o.inner_ξ()->capacity() >> 1;
        u8  b[s];
        i.close();
        while (1)
        { let r = q.t().read(fd, b, s);
          if (r <= 0
              || !(o << (η0o{}.t(η0t::bytes) << Bv{b, Sc<uN>(r)})))
            break; }
        q.t().close(fd);
        o.close(); });
  return q;
}


static Sp<ψ> ψw(Sp<ψ> q, fd_t fd, ξi i, ξo o)
{
  q->t().pin(q);
  q->name((Ss{} << ">" << fd).str())
    .def([=]() mutable
      { for (let x : i)
          if (x.type() == η0t::bytes)
          { uN  n  = 0;
            let xs = x.data();
            let s  = x.size();
            while (n < s)
            { let x = q->t().write(fd, xs + n, s - n);
              if (x == -1) { o <<= η0o(false); goto out; }
              else           o <<= η0o(x);
              n += x; } }
      out:
        q->t().unpin(q).close(fd); });
  return q;
}


struct γfd : public virtual γ
{
  γfd(fd_t fd_, bool r_) : fd(fd_), r(r_) {}

  St name() const
    { return (Ss{} << "γfd" << (r ? "<" : ">") << fd).str(); }

  Ξ &operator()(Ξ &x)
    { let q = ψfd(x.t(), fd);
      let [i, o] = x.xf(q);
      r ? ψr(q, fd, i, o) : ψw(q, fd, i, o);
      return x; }

protected:
  fd_t const fd;
  bool const r;
};


Sp<γ> γfr(fd_t f) { return Sp<γ>{new γfd(f, true)}; }
Sp<γ> γfw(fd_t f) { return Sp<γ>{new γfd(f, false)}; }


}

#include "../end.hh"
#endif
