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

  o9fdr(iN &n_, iN &e_, fd_t fd_, u32 s_ = 1 << ζb0 - 1)
    : n(n_), e(e_), fd(fd_), s(s_ - sb)
    { A(s_ > sb, "o9fdr " << s_ << "(s_) ≤ " << sb << "(sb)"); }

  uN size() const { return s + sb; }
  uN write(ζp m) const
    { n = read(fd, m + sb, s);
      if (n <= 0) { if (n == -1) e = errno; return ζω; }
      e = 0;
      W(m, 0, u9t::bytes | u9s::v32);
      W(m, 1, Sc<u32>(n));
      return n + sb; }
};

template<> struct o9_<o9fdr> { sletc v = true; };


template<>
inline void Φf<o9fdr>::init()
{
  // must use blocking IO if the FD is unable to be epolled
  if (!(ep = f << *this ? 0 : errno)) Φnb(o.fd);
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


ϝ &fd_in(Φ &f, fd_t fd)
{
  return *new ϝ(f, "fd_in", ϝ::ξΦ, [&, fd](ϝ &f, γ &g)
    { Φf<o9fdr> i{g.f, fd}; while (f << i); });
}


ϝ &fd_out(Φ &f, fd_t fd)
{
  return *new ϝ(f, "fd_out", ϝ::ξΦ, [&, fd](ϝ &f, γ &g)
    { Φf<o9fdr> o{g.f, fd};
      for (let x : f) if (!(x >> o)) break; });
}


ϝ &utf9_asm(Φ &f)
{
  return *new ϝ(f, "utf9_asm", ϝ::ξι, [](ϝ &f)
    { B b; b.reserve(1l << ζb0);
      for (let x : f)
      { if (x.τ()) b.clear();
        if (!x) continue;
        b.append(Sc<Bv>(x));
        if (b.empty()) continue;
        uN i = 0;
        for (uN s = 0;
             i < b.size()
               && i + u9sb(u9ts_s(b[i])) < b.size()
               && i + (s = i9::size_of(b.data() + i)) <= b.size();
             i += s)
          if (!(f.β() << i9{b.data() + i}))
            break;
        b.erase(0, i); }});
}


ϝ &utf9_dasm(Φ &f, uN c = 0)
{
  // NOTE: c = 0 means zero-copy and no chunking
  return !c
    ? *new ϝ(f, "utf9_dasm", ϝ::ξι, [](ϝ &f)
      { for (let x : f) { if (!(f.β() << o9q{x})) break; }})

    : *new ϝ(f, "utf9_dasm", ϝ::ξι, [c](ϝ &f)
      { B b; b.reserve(1l << ζb0);
        { for (let x : f)
          { let o = o9(x);
            let s = o.size();
            let i = b.size();
            b.resize(i + s);
            if (let n = o.write(b.data() + i))
            { A(n != ζω, "utf9_dasm write failure");
              b.resize(b.size() + n - s); }
            if (b.size() >= c)
            { if (!(f.β() << b)) break;
              b.clear(); } }
          if (!b.empty()) f.β() << b; }});
}


ϝ &fd_io(Φ &f, fd_t fd)
{
  return *new ϝ(f, "fd_io", ϝ::ξΦ,
                [&, fd](ϝ &f, γ &g) { Φf<o9fdr> o{g.f, fd}; for (let x : f) if (!(x >> o)) break; },
                [&, fd](ϝ &f, γ &g) { Φf<o9fdr> i{g.f, fd}; while (f << i); });
}


}
}

#include "end.hh"


#endif
