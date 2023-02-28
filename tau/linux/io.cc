#include "../arch.hh"
#if τlinux

#include <fcntl.h>

#include "../psi.hh"
#include "../xiio.hh"
#include "../eta.hh"
#include "../share.hh"

#include "io.hh"
#include "../begin.hh"

namespace τ
{


static Sp<ψ> ψfd(τe &t, fd_t fd)
{
  let r = Sp<ψ>{new ψ(t)};
  r->xf([&t, fd](ψ&) { t.close(fd); });
  return r;
}


static Sp<ψ> ψr(Sp<ψ> q, fd_t fd, ξo o)
{
  q->name((Ss{} << "<" << fd).str())
    .def([fd, o, qw=Wp<ψ>{q}, &t=q->t()]() mutable
      { if (!o.inner_ξ()) return;
        let s = o.inner_ξ()->capacity() >> 1;
        u8  b[s];

        // Important: don't initiate any reads if nobody is waiting for output.
        // Otherwise we might block indefinitely, hogging the FD.
        while (o.inner_ξ())
        { let r = t.read(fd, b, s);
          if (r <= 0 || !(o << η1o(Bv{b, Sc<uN>(r)}))) break; }
        o.close(); });
  return q;
}


static Sp<ψ> ψw(Sp<ψ> q, fd_t fd, ξi i)
{
  q->t().pin(q);
  q->name((Ss{} << ">" << fd).str())
    .def([fd, i, qw=Wp<ψ>{q}, &t=q->t()]() mutable
      { for (let x : i)
          if (x.tsb())
          { uN  n  = 0;
            let xs = x.data();
            let s  = x.size();
            while (n < s)
            { let y = t.write(fd, xs + n, s - n);
              if (y == -1) goto done;
              n += y; }}
      done:
        i.close();
        if (let qs = qw.lock()) t.unpin(qs); },

      // Second λ to detect pipe error before we attempt to write, then
      // unpin this ψ early. Otherwise we'd hang open even if the ξ side
      // were to exit.
      [fd, qw=Wp<ψ>{q}, &t=q->t()]() mutable
        { if (let e  = t.eg(fd))  e->y(λs::τE);
          if (let qs = qw.lock()) t.unpin(qs); });
  return q;
}


struct Γfd : public virtual Γ_
{
  Γfd(fd_t fd_, bool r_, bool w_, bool b_)
    : fd(fd_), r(r_), w(w_), b(b_) {}

  St name() const
    { return (Ss{} << "Γfd"
              << (r ? "<" : "")
              << (w ? ">" : "")
              << (b ? "⇐" : "") << fd).str(); }

  void operator()(Ξ &x) const
    { let q = ψfd(x.t(), fd);
      ξi i;
      ξo o;
      q->t().reg(fd, r, w);
      if (b) std::tie(o, i) = x.xb(q);
      else   std::tie(i, o) = x.xf(q);
      if (r) ψr(q, fd, o); else o.close();
      if (w) ψw(q, fd, i); else i.close(); }

protected:
  fd_t const fd;
  bool const r;
  bool const w;
  bool const b;
};


Γ Γfr (fd_t f, bool b) { return new Γfd(f, true,  false, b); }
Γ Γfw (fd_t f, bool b) { return new Γfd(f, false, true,  b); }
Γ Γfrw(fd_t f, bool b) { return new Γfd(f, true,  true,  b); }


Γ Γfcat(bool τ)
{
  return Γffn(τ ? "\\<τ" : "\\<", [τ](Sp<ψ> q, ξi i, ξo o) mutable
    { for (let f : i)
      { let fn = f.st();
        let fd = open(fn.c_str(), O_RDONLY);
        A(fd != -1, "open(" << fn << ") failed: " << strerror(errno));

        if (τ)
        { for (let b : Γfr(fd)(Ξ{q->t()}).p().ri()) if (!(o << b)) return;
          if (!(o << η1o(η1sig::τ))) return; }
        else
        { η0o c{η0t::bytes};
          for (let b : Γfr(fd)(Ξ{q->t()}).p().ri())
            memcpy(c.iptr(b.size()), b.data(), b.size());
          if (!(o << c)) return; }}});
}


}

#include "../end.hh"
#endif
