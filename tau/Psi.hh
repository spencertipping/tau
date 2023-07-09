#ifndef τΨ_h
#define τΨ_h

#include "debug.hh"
#include "xiio.hh"
#include "psi.hh"
#include "Gamma.hh"

#include "begin.hh"

namespace τ
{


// Auxiliary connections for Ψ; these may be connected or disconnected.
struct Ψaux final
{
  // TODO: populate these correctly
  Ψaux(Ξc &x) {}
  ξo m;
  ξi c;
};


struct Ψ0 final
{
  F<void(ψ, ξi, Ψaux)> f;
  St                   n;
  bool                 c;  // if true, run constructor inline

  Ψ0(F<void(ψ, ξi, Ψaux)> &&f_, St n_ = "", bool c_ = false)
    : f(mo(f_)), n(n_), c(c_) {}

  void operator()(ψ q, ξi i, Ψaux a) const { f(q, i, a); }
};

struct Ψ1 final
{
  F<void(ψ, ξo, Ψaux)> f;
  St                   n;
  bool                 c;  // if true, run constructor inline

  Ψ1(F<void(ψ, ξo, Ψaux)> &&f_, St n_ = "", bool c_ = false)
    : f(mo(f_)), n(n_), c(c_) {}

  void operator()(ψ q, ξo o, Ψaux a) const { f(q, o, a); }
};

struct Ψ2 final
{
  F<void(ψ, ξi, ξo, Ψaux)> f;
  St                       n;
  bool                     c;  // if true, run constructor inline

  Ψ2(F<void(ψ, ξi, ξo, Ψaux)> &&f_, St n_ = "", bool c_ = false)
    : f(mo(f_)), n(n_), c(c_) {}

  void operator()(ψ q, ξi i, ξo o, Ψaux a) const { f(q, i, o, a); }
};

struct Ψ4 final
{
  F<void(ψ, ξi, ξo, ξo, ξi, Ψaux)> f;
  St                               n;
  bool                             c;  // if true, run constructor inline

  Ψ4(F<void(ψ, ξi, ξo, ξo, ξi, Ψaux)> &&f_, St n_ = "", bool c_ = false)
    : f(mo(f_)), n(n_), c(c_) {}

  void operator()(ψ q, ξi i, ξo o, ξo oo, ξi ii, Ψaux a) const
    { f(q, i, o, oo, ii, a); }
};


// Direction to apply a Ψ to a Ξ
enum class Ψd : char {f = 'f', r = 'r', b = 'b'};
inline O &operator<<(O &s, Ψd d) { return s << Sc<char>(d); }


// Base class to manage naming and direction delegation
struct ΓΨd_ : public virtual Γ_
{
  Ψd const d;
  St const n;

  ΓΨd_(Ψd d_, St n_) : d(d_), n(n_) {}

  St name() const
    { return d == Ψd::f ?        n
           : d == Ψd::b ? "\\" + n
           :              "|"  + n; }

  Ξ operator()(Ξc &x) const
    { ψ q(x.t(), name());
      switch (d)
      {
      case Ψd::f: return f(x, q);
      case Ψd::b: return b(x, q);
      case Ψd::r: return r(x, q);
      default:    τunreachable();
      } }

  virtual Ξ f(Ξc&, ψ) const = 0;
  virtual Ξ b(Ξc&, ψ) const = 0;
  virtual Ξ r(Ξc&, ψ) const = 0;

  ψ fn(ψ q, bool c, λf &&f) const
    { if (c) f();
      else   q.f(mo(f));
      return q; }
};


// NOTE: this class has special lifetime rules (no q.weaken()) because
// nobody consumes its output; that means it lives as long as the constructor
// function is running, or creates other λs that hold references to ψ.
struct ΓΨ0_ final : public virtual ΓΨd_
{
  Ψ0 const p;

  ΓΨ0_(Ψ0 &&p_, Ψd d_ = Ψd::f, St n_ = "") : ΓΨd_(d_, n_), p(mo(p_)) {}

  Ξ f(Ξc &x, ψ q) const  // consume input
    { A(x.fi(), name() << "(" << x << ") with no ξi→");
      fn(q, p.c, [q=q.i(x.f()), p=p, i=x.f(), a=Ψaux(x)]() { p(q, i, a); });
      return x.fx(); }

  Ξ b(Ξc &x, ψ q) const  // consume future backwards input
    { A(!x.bi(), name() << "(" << x << ") clobbers existing ξo←");
      let [o, i] = x.t().pipe();
      fn(q, p.c, [q=q.i(i), p=p, i=i, a=Ψaux(x)]() { p(q, i, a); });
      return x.b(o); }

  Ξ r(Ξc &x, ψ q) const { return f(x, q).bx(); }
};


struct ΓΨ1_ final : public virtual ΓΨd_
{
  Ψ1 const p;

  ΓΨ1_(Ψ1 &&p_, Ψd d_ = Ψd::f, St n_ = "") : ΓΨd_(d_, n_), p(mo(p_)) {}

  Ξ f(Ξc &x, ψ q) const  // produce future forwards input
    { A(!x.fi(), name() << "(" << x << ") clobbers existing ξi→");
      let [o, i] = x.t().pipe();
      fn(q, p.c, [q=q.o(o), p=p, o=o, a=Ψaux(x)]() mutable { q.weaken(true); p(q, o, a); });
      return x.f(i); }

  Ξ b(Ξc &x, ψ q) const  // produce backwards output
    { A(x.bi(), name() << "(" << x << ") with closed ξo←");
      fn(q, p.c, [q=q.o(x.b()), p=p, o=x.b(), a=Ψaux(x)]() mutable { q.weaken(true); p(q, o, a); });
      return x.bx(); }

  Ξ r(Ξc &x, ψ q) const { return b(x, q).fx(); }
};


struct ΓΨ2_ final : public virtual ΓΨd_
{
  Ψ2 const p;

  ΓΨ2_(Ψ2 &&p_, Ψd d_ = Ψd::f, St n_ = "") : ΓΨd_(d_, n_), p(mo(p_)) {}

  Ξ f(Ξc &x, ψ q) const  // transform forwards
    { let [o, i] = x.t().pipe();
      fn(q.i(x.f()).o(o), p.c,
         [q=q, p=p, i=x.f(), o=o, a=Ψaux(x)]() mutable { q.weaken(true); p(q, i, o, a); });
      return x.f(i); }

  Ξ b(Ξc &x, ψ q) const  // transform backwards
    { let [o, i] = x.t().pipe();
      fn(q.i(i).o(x.b()), p.c,
         [q=q, p=p, i=i, o=x.b(), a=Ψaux(x)]() mutable { q.weaken(true); p(q, i, o, a); });
      return x.b(o); }

  Ξ r(Ξc &x, ψ q) const
    { fn(q.i(x.f()).o(x.b()), p.c,
         [q=q, p=p, i=x.f(), o=x.b(), a=Ψaux(x)]() mutable { q.weaken(true); p(q, i, o, a); });
      return x.fx().bx(); }
};


struct ΓΨ4_ final : public virtual ΓΨd_
{
  Ψ4 const p;

  ΓΨ4_(Ψ4 &&p_, Ψd d_ = Ψd::f, St n_ = "") : ΓΨd_(d_, n_), p(mo(p_))
    { A(d != Ψd::r, "cannot apply Ψ4:r"); }

  Ξ f(Ξc &x, ψ q) const  // transform forwards
    { let [fo, fi] = x.t().pipe();
      let [bo, bi] = x.t().pipe();
      fn(q.i(x.f()).i(bi).o(x.b()).o(fo), p.c,
         [q=q, p=p, fi=x.f(), fo=fo, bo=x.b(), bi=bi, a=Ψaux(x)]() mutable
           { q.weaken(true);
             p(q, fi, fo, bo, bi, a); });
      return x.f(fi).b(bo); }

  Ξ b(Ξc &x, ψ q) const  // transform backwards
    { let [fo, fi] = x.t().pipe();
      let [bo, bi] = x.t().pipe();
      fn(q.i(x.f()).i(bi).o(x.b()).o(bo), p.c,
         [q=q, p=p, fi=x.f(), fo=fo, bo=x.b(), bi=bi, a=Ψaux(x)]() mutable
           { q.weaken(true);
             p(q, bi, bo, fo, fi, a); });
      return x.f(fi).b(bo); }

  Ξ r(Ξc &x, ψ q) const { τunreachable(); }
};


inline Γ ΓΨ(Ψ0 &&p, Ψd d = Ψd::f, St n = "") { return {new ΓΨ0_(mo(p), d, n)}; }
inline Γ ΓΨ(Ψ1 &&p, Ψd d = Ψd::f, St n = "") { return {new ΓΨ1_(mo(p), d, n)}; }
inline Γ ΓΨ(Ψ2 &&p, Ψd d = Ψd::f, St n = "") { return {new ΓΨ2_(mo(p), d, n)}; }
inline Γ ΓΨ(Ψ4 &&p, Ψd d = Ψd::f, St n = "") { return {new ΓΨ4_(mo(p), d, n)}; }


// Shorthand ways to create Ψs, i.e. with less function-arg overhead: Ψ is normal,
// Ψc runs the constructor inline.
inline Ψ0 Ψ (F<void(ξi)>          &&f, St n = "") { return {[f=mo(f)](ψ,   ξi i, Ψaux) { return f(i); }, n}; }
inline Ψ0 Ψ (F<void(ψ, ξi)>       &&f, St n = "") { return {[f=mo(f)](ψ q, ξi i, Ψaux) { return f(q, i); }, n}; }
inline Ψ0 Ψ (F<void(ψ, ξi, Ψaux)> &&f, St n = "") { return {mo(f), n}; }
inline Ψ0 Ψc(F<void(ψ, ξi)>       &&f, St n = "") { return {[f=mo(f)](ψ q, ξi i, Ψaux) { return f(q, i); }, n, true}; }
inline Ψ0 Ψc(F<void(ψ, ξi, Ψaux)> &&f, St n = "") { return {mo(f), n, true}; }

inline Ψ1 Ψ (F<void(ξo)>          &&f, St n = "") { return {[f=mo(f)](ψ,   ξo o, Ψaux) { return f(o); }, n}; }
inline Ψ1 Ψ (F<void(ψ, ξo)>       &&f, St n = "") { return {[f=mo(f)](ψ q, ξo o, Ψaux) { return f(q, o); }, n}; }
inline Ψ1 Ψ (F<void(ψ, ξo, Ψaux)> &&f, St n = "") { return {mo(f), n}; }
inline Ψ1 Ψc(F<void(ψ, ξo)>       &&f, St n = "") { return {[f=mo(f)](ψ q, ξo o, Ψaux) { return f(q, o); }, n, true}; }
inline Ψ1 Ψc(F<void(ψ, ξo, Ψaux)> &&f, St n = "") { return {mo(f), n, true}; }

inline Ψ2 Ψ (F<void(ξi, ξo)>          &&f, St n = "") { return {[f=mo(f)](ψ,   ξi i, ξo o, Ψaux) { return f(i, o); }, n}; }
inline Ψ2 Ψ (F<void(ψ, ξi, ξo)>       &&f, St n = "") { return {[f=mo(f)](ψ q, ξi i, ξo o, Ψaux) { return f(q, i, o); }, n}; }
inline Ψ2 Ψ (F<void(ψ, ξi, ξo, Ψaux)> &&f, St n = "") { return {mo(f), n}; }
inline Ψ2 Ψc(F<void(ψ, ξi, ξo)>       &&f, St n = "") { return {[f=mo(f)](ψ q, ξi i, ξo o, Ψaux) { return f(q, i, o); }, n, true}; }
inline Ψ2 Ψc(F<void(ψ, ξi, ξo, Ψaux)> &&f, St n = "") { return {mo(f), n, true}; }

inline Ψ4 Ψ (F<void(ξi, ξo, ξo, ξi)>          &&f, St n = "") { return {[f=mo(f)](ψ,   ξi fi, ξo fo, ξo bo, ξi bi, Ψaux) { return f(fi, fo, bo, bi); }, n}; }
inline Ψ4 Ψ (F<void(ψ, ξi, ξo, ξo, ξi)>       &&f, St n = "") { return {[f=mo(f)](ψ q, ξi fi, ξo fo, ξo bo, ξi bi, Ψaux) { return f(q, fi, fo, bo, bi); }, n}; }
inline Ψ4 Ψ (F<void(ψ, ξi, ξo, ξo, ξi, Ψaux)> &&f, St n = "") { return {mo(f), n}; }
inline Ψ4 Ψc(F<void(ψ, ξi, ξo, ξo, ξi)>       &&f, St n = "") { return {[f=mo(f)](ψ q, ξi fi, ξo fo, ξo bo, ξi bi, Ψaux) { return f(q, fi, fo, bo, bi); }, n, true}; }
inline Ψ4 Ψc(F<void(ψ, ξi, ξo, ξo, ξi, Ψaux)> &&f, St n = "") { return {mo(f), n, true}; }


}

#include "end.hh"

#endif
