#include "prof.hh"
#include "begin.hh"

namespace σ
{

using namespace τ;


static M<ηm, Sp<measurement>> measurements_;
static Mu                     measurements_mu_;


void Γprof(Γφ &g)
{
  g.def_p1("prof", [](f64 s, ψ q, ξo o)
    {
      i64 ns = s * 1e9;
      while (o)
      {
        M<ηm, Sp<measurement>> m;
        {
          Ul<Mu> l{measurements_mu_};
          m = measurements_;
        }

        V<ηm> ks;
        for (let &[k, v] : m) ks.push_back(k);
        std::sort(ks.begin(), ks.end(),
                  [&](ηmc &a, ηmc &b)
                  { return m[a]->real() < m[b]->real(); });

        for (let &k : ks)
        {
          let v = m[k];
          Ss s;
          s << v->n()          << "x  "
            << v->duty() * 100 << "%  "
            << v->real()       << " real  "
            << v->mean()       << " mean  "
            << v->min()        << " min  "
            << v->max()        << " max  "
            << v->down()       << " down  "
            << v->gap()        << " gap  "
            << v->active()     << " active";

          o.r() << k << s.str();
        }

        o.r() << ηsig::τ;

        q.t().Θ(now() + ns * 1ns);
      }
    });
}


Sp<measurement> measurement_for(ηmc &k)
{
  Ul<Mu> l{measurements_mu_};
  let i = measurements_.find(k);
  if (i != measurements_.end()) return i->second;

  let m = measurements_[k] = Sp<measurement>{new measurement};
  m->self_ = m;
  return m;
}

Sp<measurement> measurement_for(Stc &k)
{
  return measurement_for(ηm{} << k);
}


M<ηm, Sp<measurement>> measurements()
{
  Ul<Mu> l{measurements_mu_};
  return measurements_;
}


timer prof(ηmc &k) { return measurement_for(k)->start(); }
timer prof(Stc &k) { return measurement_for(k)->start(); }


ΔΘ timer::stop()
{
  let t = now() - start_;
  if (m_) m_->stop(t);
  m_ = nullptr;
  return t;
}


timer measurement::start()
{
  Ul<Smu> l{mu_};
  if (!active_++)
  {
    down_ += now() - last_;
    since_ = now();
  }
  return {self_.lock()};
}


void measurement::stop(ΔΘ dt)
{
  let n = now();

  Ul<Smu> l{mu_};
  A(active_ > 0, "measurement::stop() called without matching start()");

  ++n_;
  virtual_ += dt;
  if (dt < min_) min_ = dt;
  if (dt > max_) max_ = dt;
  if (!--active_)
  {
    real_ += n - since_;
    since_ = never();
    last_  = n;
  }
}


ΔΘ measurement::real() const
{
  Sl<Smu> l{mu_};
  return real_ + (active_ ? now() - since_ : 0ns);
}


ΔΘ measurement::down() const
{
  Sl<Smu> l{mu_};
  return down_ + (active_ ? 0ns : now() - last_);
}


O &operator<<(O &s, measurement const &m)
{
  return s << m.n()          << "x  "
           << m.duty() * 100 << "%  "
           << m.real()       << " real  "
           << m.mean()       << " mean  "
           << m.min()        << " min  "
           << m.max()        << " max  "
           << m.down()       << " down  "
           << m.gap()        << " gap  "
           << m.active()     << " active";
}

O &operator<<(O &s, Sp<measurement> m)
{
  return s << *m;
}


}


#include "end.hh"
