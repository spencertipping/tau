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
  Ψd const d;
  St const n;

  ΓΨd_(Ψd d_, St n_) : d(d_), n(n_) {}

  St name() const { return (Ss{} << n << ":" << d).str(); }

  Ξ operator()(Ξ const &x) const
    { ψ q(x.t(), name());
      switch (d)
      {
      case Ψd::f: return f(x, q);
      case Ψd::b: return b(x, q);
      case Ψd::r: return r(x, q);
      } }

  virtual Ξ f(Ξ const&, ψ) const = 0;
  virtual Ξ b(Ξ const&, ψ) const = 0;
  virtual Ξ r(Ξ const&, ψ) const = 0;
};


struct ΓΨ0 final : public virtual ΓΨd_
{
  Ψ0 const p;

  ΓΨ0(Ψ0 &&p_, Ψd d_ = Ψd::f, St n_ = "") : ΓΨd_(d_, n_), p(std::move(p_)) {}

  Ξ f(Ξ const &x, ψ q) const  // consume input
    { A(x.fi(), name() << "(" << x << ") with no ξi→");
      q.f([q=q, p=p, i=x.f()]() { p(q, i, {}); });
      return x.f({}); }

  Ξ b(Ξ const &x, ψ q) const  // consume future backwards input
    { A(!x.bi(), name() << "(" << x << ") clobbers existing ξo←");
      let [o, i] = x.t().pipe();
      q.f([q=q, p=p, i=i]() { p(q, i, {}); });
      return x.b(o); }

  Ξ r(Ξ const &x, ψ q) const { return f(x, q).b({}); }
};


struct ΓΨ1 final : public virtual ΓΨd_
{
  Ψ1 const p;

  ΓΨ1(Ψ1 &&p_, Ψd d_ = Ψd::f, St n_ = "") : ΓΨd_(d_, n_), p(std::move(p_)) {}

  Ξ f(Ξ const &x, ψ q) const  // produce future forwards input
    { A(!x.fi(), name() << "(" << x << ") clobbers existing ξi→");
      let [o, i] = x.t().pipe();
      q .f ([q=q, p=p, o=o]()    { p(q, o, {}); o.close(); })
        .fx([o=o]          (ψ_&) {              o.close(); });
      return x.f(i); }

  Ξ b(Ξ const &x, ψ q) const  // produce backwards output
    { A(x.bi(), name() << "(" << x << ") with closed ξo←");
      q .f ([q=q, p=p, o=x.b()]()    { p(q, o, {}); o.close(); })
        .fx([o=x.b()]          (ψ_&) {              o.close(); });
      return x.b({}); }

  Ξ r(Ξ const &x, ψ q) const { return b(x, q).f({}); }
};


struct ΓΨ2 final : public virtual ΓΨd_
{
  Ψ2 const p;

  ΓΨ2(Ψ2 &&p_, Ψd d_ = Ψd::f, St n_ = "") : ΓΨd_(d_, n_), p(std::move(p_)) {}

  Ξ f(Ξ const &x, ψ q) const  // transform forwards
    { let [o, i] = x.t().pipe();
      q .f ([q=q, p=p, i=x.f(), o=o]()    { p(q, i, o, {}); o.close(); })
        .fx([o=o]                   (ψ_&) {                 o.close(); });
      return x.f(i); }

  Ξ b(Ξ const &x, ψ q) const  // transform backwards
    { let [o, i] = x.t().pipe();
      q .f ([q=q, p=p, i=i, o=x.b()]()    { p(q, i, o, {}); o.close(); })
        .fx([o=x.b()]               (ψ_&) {                 o.close(); });
      return x.b(o); }

  Ξ r(Ξ const &x, ψ q) const
    { q .f ([q=q, p=p, i=x.f(), o=x.b()]()    { p(q, i, o, {}); o.close(); })
        .fx([o=x.b()]                   (ψ_&) {                 o.close(); });
      return x.f({}).b({}); }
};


struct ΓΨ4 final : public virtual ΓΨd_
{
  Ψ4 const p;

  ΓΨ4(Ψ4 &&p_, Ψd d_ = Ψd::f, St n_ = "") : ΓΨd_(d_, n_), p(std::move(p_))
    { A(d != Ψd::r, "cannot apply Ψ4:r"); }

  Ξ f(Ξ const &x, ψ q) const  // transform forwards
    { let [fo, fi] = x.t().pipe();
      let [bo, bi] = x.t().pipe();
      q .f ([q=q, p=p, fi=x.f(), fo=fo, bo=x.b(), bi=bi]()
          { p(q, fi, fo, bo, bi, {}); fo.close(); bo.close(); })
        .fx([fo=fo, bo=x.b()] (ψ_&) { fo.close(); bo.close(); });
      return x.f(fi).b(bo); }

  Ξ b(Ξ const &x, ψ q) const  // transform backwards
    { let [fo, fi] = x.t().pipe();
      let [bo, bi] = x.t().pipe();
      q .f ([q=q, p=p, fi=x.f(), fo=fo, bo=x.b(), bi=bi]()
          { p(q, bi, bo, fo, fi, {}); fo.close(); bo.close(); })
        .fx([fo=fo, bo=x.b()] (ψ_&) { fo.close(); bo.close(); });
      return x.f(fi).b(bo); }

  Ξ r(Ξ const &x, ψ q) const { τunreachable(); }
};


}

#include "end.hh"

#endif
