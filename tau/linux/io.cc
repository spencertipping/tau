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


static Sp<ψ> ψr(Sp<ψ> q, fd_t fd, ξi i, ξo o)
{
  q->t().reg(fd, true, false);
  q->name((Ss{} << "<" << fd).str())
    .def([fd, i, o, qw=Wp<ψ>{q}, &t=q->t()]() mutable
      { if (!o.inner_ξ()) return;
        let s = o.inner_ξ()->capacity() >> 1;
        u8  b[s];
        i.close();
        while (o.inner_ξ())
        { let r = t.read(fd, b, s);
          if (r <= 0 || !(o << η1o(Bv{b, Sc<uN>(r)}))) break; }
        o.close(); });
  return q;
}


static Sp<ψ> ψw(Sp<ψ> q, fd_t fd, ξi i, ξo o)
{
  q->t().reg(fd, false, true);
  q->t().pin(q);
  q->name((Ss{} << ">" << fd).str())
    .def([fd, i, o, qw=Wp<ψ>{q}, &t=q->t()]() mutable
      { for (let x : i)
          if (x.tsb())
          { uN  n  = 0;
            let xs = x.data();
            let s  = x.size();
            while (n < s)
            { let y = t.write(fd, xs + n, s - n);
              if (y == -1) { o <<= η1o(false); goto done; }
              else           o <<= η1o(y);
              n += y; }}
      done:
        i.close();
        o.close();
        if (let qs = qw.lock()) t.unpin(qs); },

      // Second λ to detect pipe error before we attempt to write, then
      // unpin this ψ early. Otherwise we'd hang open even if the ξ side
      // were to exit.
      [fd, qw=Wp<ψ>{q}, &t=q->t()]() mutable
        { t.eg(fd)->y(λs::τE);
          if (let qs = qw.lock()) t.unpin(qs); });
  return q;
}


struct γfd : public virtual γ_
{
  γfd(fd_t fd_, bool r_) : fd(fd_), r(r_) {}

  St name() const
    { return (Ss{} << "γfd" << (r ? "<" : ">") << fd).str(); }

  void operator()(Ξ &x) const
    { let q = ψfd(x.t(), fd);
      let [i, o] = x.xf(q);
      r ? ψr(q, fd, i, o) : ψw(q, fd, i, o); }

protected:
  fd_t const fd;
  bool const r;
};


γ γfr(fd_t f) { return new γfd(f, true); }
γ γfw(fd_t f) { return new γfd(f, false); }


γ γfcat(bool τ)
{
  return γffn(τ ? "\\<τ" : "\\<", [τ](Sp<ψ> q, ξi i, ξo o) mutable
    { for (let f : i)
      { let fn = f.st();
        let fd = open(fn.c_str(), O_RDONLY);
        A(fd != -1, "open(" << fn << ") failed: " << strerror(errno));

        if (τ)
        { for (let b : γfr(fd)(Ξ{q->t()}).p().fi()) if (!(o << b)) return;
          if (!(o << η1o(η1sig::τ))) return; }
        else
        { η0o c{η0t::bytes};
          for (let b : γfr(fd)(Ξ{q->t()}).p().fi())
            memcpy(c.iptr(b.size()), b.data(), b.size());
          if (!(o << c)) return; }}});
}


}

#include "../end.hh"
#endif
