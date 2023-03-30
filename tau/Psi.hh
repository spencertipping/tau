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
  Ψaux(Ξc &x) : m(x.t().l(), {}) {}
  ξo m;
  ξi c;
};


typedef F<void(ψ, ξi,             Ψaux)> Ψ0;
typedef F<void(ψ,     ξo,         Ψaux)> Ψ1;
typedef F<void(ψ, ξi, ξo,         Ψaux)> Ψ2;
typedef F<void(ψ, ξi, ξo, ξo, ξi, Ψaux)> Ψ4;


// Direction to apply a Ψ to a Ξ
enum class Ψd : char {f = 'f', r = 'r', b = 'b'};
inline O &operator<<(O &s, Ψd d) { return s << Sc<char>(d); }


// Base class to manage naming and direction delegation
struct ΓΨd_ : public virtual Γ_
{
  Ψd   const d;
  St   const n;
  bool const c;  // if true, run constructor inline

  ΓΨd_(Ψd d_, St n_, bool c_) : d(d_), n(n_), c(c_) {}

  St name() const { return (Ss{} << n << ":" << d).str(); }

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

  ψ fn(ψ q, λf &&f) const
    { if (c) f();
      else   q.f(std::move(f));
      return q; }
};


struct ΓΨ0 final : public virtual ΓΨd_
{
  Ψ0 const p;

  ΓΨ0(Ψ0 &&p_, Ψd d_ = Ψd::f, St n_ = "", bool c_ = false)
    : ΓΨd_(d_, n_, c_), p(std::move(p_)) {}

  Ξ f(Ξ const &x, ψ q) const  // consume input
    { A(x.fi(), name() << "(" << x << ") with no ξi→");
      fn(q, [q=q.i(x.f()), p=p, i=x.f(), a=Ψaux(x)]() { p(q, i, a); });
      return x.fx(); }

  Ξ b(Ξ const &x, ψ q) const  // consume future backwards input
    { A(!x.bi(), name() << "(" << x << ") clobbers existing ξo←");
      let [o, i] = x.t().pipe();
      fn(q, [q=q.i(i), p=p, i=i, a=Ψaux(x)]() { p(q, i, a); });
      return x.b(o); }

  Ξ r(Ξ const &x, ψ q) const { return f(x, q).bx(); }
};


struct ΓΨ1 final : public virtual ΓΨd_
{
  Ψ1 const p;

  ΓΨ1(Ψ1 &&p_, Ψd d_ = Ψd::f, St n_ = "", bool c_ = false)
    : ΓΨd_(d_, n_, c_), p(std::move(p_)) {}

  Ξ f(Ξ const &x, ψ q) const  // produce future forwards input
    { A(!x.fi(), name() << "(" << x << ") clobbers existing ξi→");
      let [o, i] = x.t().pipe();
      fn(q, [q=q.o(o), p=p, o=o, a=Ψaux(x)]() { p(q, o, a); });
      return x.f(i); }

  Ξ b(Ξ const &x, ψ q) const  // produce backwards output
    { A(x.bi(), name() << "(" << x << ") with closed ξo←");
      fn(q, [q=q.o(x.b()), p=p, o=x.b(), a=Ψaux(x)]() { p(q, o, a); });
      return x.bx(); }

  Ξ r(Ξ const &x, ψ q) const { return b(x, q).fx(); }
};


struct ΓΨ2 final : public virtual ΓΨd_
{
  Ψ2 const p;

  ΓΨ2(Ψ2 &&p_, Ψd d_ = Ψd::f, St n_ = "", bool c_ = false)
    : ΓΨd_(d_, n_, c_), p(std::move(p_)) {}

  Ξ f(Ξ const &x, ψ q) const  // transform forwards
    { let [o, i] = x.t().pipe();
      fn(q.i(x.f()).o(o),
         [q=q, p=p, i=x.f(), o=o, a=Ψaux(x)]() { p(q, i, o, a); });
      return x.f(i); }

  Ξ b(Ξ const &x, ψ q) const  // transform backwards
    { let [o, i] = x.t().pipe();
      fn(q.i(i).o(x.b()),
         [q=q, p=p, i=i, o=x.b(), a=Ψaux(x)]() { p(q, i, o, a); });
      return x.b(o); }

  Ξ r(Ξ const &x, ψ q) const
    { fn(q.i(x.f()).o(x.b()),
         [q=q, p=p, i=x.f(), o=x.b(), a=Ψaux(x)]() { p(q, i, o, a); });
      return x.fx().bx(); }
};


struct ΓΨ4 final : public virtual ΓΨd_
{
  Ψ4 const p;

  ΓΨ4(Ψ4 &&p_, Ψd d_ = Ψd::f, St n_ = "", bool c_ = false)
    : ΓΨd_(d_, n_, c_), p(std::move(p_))
    { A(d != Ψd::r, "cannot apply Ψ4:r"); }

  Ξ f(Ξ const &x, ψ q) const  // transform forwards
    { let [fo, fi] = x.t().pipe();
      let [bo, bi] = x.t().pipe();
      fn(q.i(x.f()).i(bi).o(x.b()).o(fo),
         [q=q, p=p, fi=x.f(), fo=fo, bo=x.b(), bi=bi, a=Ψaux(x)]()
           { p(q, fi, fo, bo, bi, a); });
      return x.f(fi).b(bo); }

  Ξ b(Ξ const &x, ψ q) const  // transform backwards
    { let [fo, fi] = x.t().pipe();
      let [bo, bi] = x.t().pipe();
      fn(q.i(x.f()).i(bi).o(x.b()).o(bo),
         [q=q, p=p, fi=x.f(), fo=fo, bo=x.b(), bi=bi, a=Ψaux(x)]()
           { p(q, bi, bo, fo, fi, a); });
      return x.f(fi).b(bo); }

  Ξ r(Ξ const &x, ψ q) const { τunreachable(); }
};


}

#include "end.hh"

#endif
