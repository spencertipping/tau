#include "Omega-io.hh"
#include "../begin.hh"

namespace σ
{

using namespace τ;


struct Ωss_frag_ final : public virtual Ωss
{
  Ωss_frag_(Sp<Ωf> f, u64 o, u64 s)
    : Ωss(), f_(f), o_(o), i_(0), n_(s / Ωhm_size) {}

  Ωss     &operator++()       override { ++i_; return *this; }
  u64        max_size() const override { return n_; }
  operator       bool() const override { return i_ < n_; }
  P<Ωh, Ωp> operator*() const override
  {
    return {f_->get<u64b>(o_ + i_ * Ωhm_size),
            f_->get<u64b>(o_ + i_ * Ωhm_size + 8)};
  }

protected:
  Sp<Ωf> f_;
  u64    o_;
  u64    i_;
  u64c   n_;
};


struct Ωss_merge_ final : public virtual Ωss
{
  Ωss_merge_(Sp<Ωss> a, Sp<Ωss> b) : Ωss(), a_(a), b_(b) {}

  SO ord() const
  {
    if (!*a_) return SO::greater;
    if (!*b_) return SO::less;
    return **a_ <=> **b_;
  }

  operator bool() const override { return *a_ || *b_; }
  u64  max_size() const override { return a_->max_size() + b_->max_size(); }

  Ωss &operator++() override
  {
    if (!*a_) return ++*b_;
    if (!*b_) return ++*a_;
    return ord() == SO::less ? ++*a_ : ++*b_;
  }

  P<Ωh, Ωp> operator*() const override
  {
    if (!*a_) return **b_;
    if (!*b_) return **a_;
    return ord() == SO::less ? **a_ : **b_;
  }

protected:
  Sp<Ωss> a_;
  Sp<Ωss> b_;
};


}

#include "../end.hh"
