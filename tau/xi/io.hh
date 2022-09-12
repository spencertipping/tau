#ifndef τξio_h
#define τξio_h


#include "begin.hh"


namespace τ
{


struct o9fdr  // zero-copy read from FD
{
  sletc sb = u9sb(u9s::v32);  static_assert(sb == 5);
  iN   &n;    // NOTE: must be external to this o9, since o9 objects
  iN   &e;    // are assumed to be trivially copyable/destructible
  uNc   fd;
  u32c  s;

  o9fdr(iN &n_, iN &e_, uN fd_, u32 s_ = 1 << ζb0 - 1)
    :  n(n_), e(e_), fd(fd_), s(s_ - sb)
    { A(s_ > sb, "o9fdr " << s_ << "(s_) ≤ " << sb << "(sb)"); }

  uN size() const { return s + sb; }
  uN write(ζp m) const
    { n = read(fd, m + sb, s);
      if (n <= 0) { if (n == -1) e = errno; return ζω; }
      e = 0;
      W<u8>(m, 0, u9t::bytes | u9s::v32);
      W<u32>(m, 1, n);
      return n + sb; }
};

template<> struct o9_<o9fdr> { sletc v = true; };


template<>
inline void Φf<o9fdr>::init()
{
  Φnb(o.fd);
  ep = f << *this ? 0 : errno;
}

template<>
inline Φf<o9fdr>::~Φf()
{
  f.x(*this);
}


bool operator>>(i9 v, Φf<o9fdr> &w)
{
  w.wo = 0;
  while (w.wo < v.size())
  {
    if (!w.ep) while (!w.wa()) w.w.y(λs::ΦO);
    if ((w.wn = write(w.fd(), v.begin() + w.wo, v.size() - w.wo)) == -1)
    { if ((w.we = errno) != EAGAIN)
        return false; }
    else
      w.wo += w.wn;
  }
  return true;
}


namespace ξ
{


ϝ &fd_in(Φ &f, uN fd)
{
  return *new ϝ(f, ϝ::ξΦ, [&, fd](ϝ &f, γ &g)
    { Φf<o9fdr> i{g.f, fd}; while (f << i); });
}


ϝ &fd_out(Φ &f, uN fd)
{
  return *new ϝ(f, ϝ::ξΦ, [&, fd](ϝ &f, γ &g)
    { Φf<o9fdr> o{g.f, fd};
      for (let x : f) if (!(x >> o)) break; });
}


ϝ &fd_io(Φ &f, uN fd)
{
  return *new ϝ(f, ϝ::ξΦ,
                [&, fd](ϝ &f, γ &g) { Φf<o9fdr> o{g.f, fd}; for (let x : f) if (!(x >> o)) break; },
                [&, fd](ϝ &f, γ &g) { Φf<o9fdr> i{g.f, fd}; while (f << i); });
}


}
}

#include "end.hh"


#endif
