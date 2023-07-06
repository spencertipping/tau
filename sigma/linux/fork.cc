#include "fork.hh"
#include "io.hh"
#include "../quote.hh"
#include "../begin.hh"
#include <cstring>

namespace σ
{


P<fd_t, fd_t> pipe_()
{
  int fds[2];
  A(!pipe(fds), "pipe() failed: " << strerror(errno));
  std::cerr << "pipe_ : <" << fds[0] << " >" << fds[1] << std::endl;
  return mp(fds[0], fds[1]);
}


void Γfork(Γφ &g)
{
  g .def_g("P2", τfork2)
    .def_g("P4", τfork4);
}


// Read quoted file (so unquote as we read)
static Γ rqf(fd_t f, Ψd d = Ψd::f)
{
  return d == Ψd::f
       ? ΓrF(f) | unquote()
       : unquote(d) | ΓrF(f, d);
}

// Write quoted file
static Γ wqf(fd_t f, Ψd d = Ψd::f)
{
  return d == Ψd::f
       ? quote() | ΓwF(f)
       : ΓwF(f, d) | quote(d);
}


// Half-duplex fork: connect as a Ψ2
Ξ τfork2(Γa<Γ> g, Ξc &x)
{
  // NOTE: these variables are from the child's perspective;
  // that is, fo means "child's forward out"
  let [fiR, fiW] = pipe_();
  let [foR, foW] = pipe_();

  let child = x.t().fork();
  A(child != -1, "P2 fork(" << g.x << ") failed: " << strerror(errno));

  if (!child)
  {
    // Child process reads from fi, writes to fo,
    // reads from bi, and writes to bo.
    x.t().detach();
    close(fiW); close(foR);
    τe t1;
    (rqf(fiR) | g.x | wqf(foW))(Ξ{t1}.push());
    t1.go();
    exit(0);
  }
  else
  {
    close(fiR); close(foW);
    return (wqf(fiW) | rqf(foR))(x);
  }
}


// Full-duplex fork: connect as a Ψ4
Ξ τfork4(Γa<Γ> g, Ξc &x)
{
  // NOTE: these variables are from the child's perspective;
  // that is, fo means "child's forward out"
  let [fiR, fiW] = pipe_();
  let [foR, foW] = pipe_();
  let [biR, biW] = pipe_();
  let [boR, boW] = pipe_();

  let child = x.t().fork();
  A(child != -1, "P4 fork(" << g.x << ") failed: " << strerror(errno));

  if (!child)
  {
    // Child process reads from fi, writes to fo,
    // reads from bi, and writes to bo.
    x.t().detach();
    close(fiW); close(foR);
    close(biW); close(boR);
    let l = rqf(fiR) | wqf(boR, Ψd::b);
    let r = wqf(foW) | rqf(biR, Ψd::b);
    τe t1;
    (l | g.x | r)(Ξ{t1}.push());
    t1.go();
    exit(0);
  }
  else
  {
    close(fiR); close(foW);
    close(biR); close(boW);
    let l = wqf(fiW) | rqf(boW, Ψd::b);
    let r = rqf(foR) | wqf(biW, Ψd::b);
    return (l | r)(x);
  }
}


}

#include "../end.hh"
