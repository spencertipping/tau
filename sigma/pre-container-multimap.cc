#include "pre-container-multimap.hh"
#include "begin.hh"

namespace σ::pre
{


void kvmmat_::α(ηic &k, ηic &v, ξo)
{
  astage(k, v);
}

void kvmmat_::ω(ηic &k, ηic &v, ξo)
{
  if (v.empty()) del(k);
  else           dstage(k, v);
}

void kvmmat_::ι(ηic &k, ηic&, ξo o)
{
  auto i = at(k);
  while (*i) o.r() << **i, ++*i;
  o.r() << ηsig::τ;
}

void kvmmat_::κ(ηic &k, ηic&, ξo o)
{
  flush(k);
}

void kvmmat_::τ(ηic &x, ξo o)
{
  flush();
  o.r() << ηsig::τ;
}


void kvmmat_::astage(ηic &k, ηic &v)
{
  if (add_[k].insert(v).second) ss_ += v.lsize();
  let i = del_.find(k);
  if (i != del_.end()) if (i->second.erase(v)) ss_ -= v.lsize();
  post_stage(k);
}

void kvmmat_::dstage(ηic &k, ηic &v)
{
  if (del_[k].insert(v).second) ss_ += v.lsize();
  let i = add_.find(k);
  if (i != add_.end()) if (i->second.erase(v)) ss_ -= v.lsize();
  post_stage(k);
}

void kvmmat_::merge(ηic &k)
{
  if (!kv_has(k) || !kv_is_indirect(k)) return;

  // Merge all indirect keys plus the stage down to a single new key
  let ks = kv_indirect(k);  // save these so we can delete them later
  if (ks.size() == 1 && !has_add(k) && !has_del(k)) return;

  let ik = genkey();
  db_->set(ikey(ik), mo(*at(k)).all());
  db_->set(lkey(k), ηm{} << 1 << ik.all());
  for (let &i : ks) db_->del(ikey(i));

  // Remove from stage
  if (add_.contains(k)) for (let &a : add_.at(k)) ss_ -= a.lsize();
  if (del_.contains(k)) for (let &d : del_.at(k)) ss_ -= d.lsize();
  A(ss_ >= 0, "kvmmat_::merge: bad stage accounting: ss_ = " << ss_);
}

ηm kvmmat_::flush(ηic &k)
{
  if (!add_.contains(k) && !del_.contains(k)) return ηm{};

  if (!kv_has(k))  // nothing in DB → insert literal
  {
    auto s = at(k);
    if (!*s) return ηm{};  // nothing to do
    db_->set(lkey(k), ηm{} << 0 << mo(*s).all().all());
    return ηm{};
  }
  else if (kv_is_indirect(k))
  {
    // Already indirect, so let's append an entry to it. If we end up with
    // too many keys, merge the indirects.
    let ik = genkey();

  }
}

void kvmmat_::flush()
{
  V<ηm> ks; ks.reserve(add_.size() + del_.size());

}


bool kvmmat_::has_add(ηic &k) const
{
  let i = add_.find(k);
  return i != add_.end() && !i->second.empty();
}

bool kvmmat_::has_del(ηic &k) const
{
  let i = del_.find(k);
  return i != del_.end() && !i->second.empty();
}


bool kvmmat_::kv_has(ηic &k) const
{
  return db_->has(lkey(k));
}

ηm kvmmat_::genkey()
{
  ++*(long*)nk_.data();  // optimize for common case (sparse entries)
  while (db_->has(ikey(nk_)))
  {
    // Less-common case: key collision, so scramble it with lots of entropy
    *(long*)nk_.data() += now().time_since_epoch().count();
    nk_ = (isha2{} << nk_)();
  }
  return ηm{sizeof(h256) + 3} << nk_;
}

bool kvmmat_::kv_is_indirect(ηic &k) const
{
  if constexpr (τdebug)
                 A(kv_has(k), "kvmmat_::is_indirect: key " << k << " not found");
  return !!db_->get(lkey(k)).i();
}

uN kvmmat_::kv_ilen(ηic &k) const
{
  if (!kv_is_indirect(k)) return 0;
  return db_->get(lkey(k)).len() - 1;
}

ηi kvmmat_::kv_literal(ηic &k) const
{
  return db_->get(lkey(k)).next();
}

V<ηm> kvmmat_::kv_indirect(ηic &k) const
{
  V<ηm> r;
  for (auto i = db_->get(lkey(k)).next(); i.has_next(); ++i, ++i)
    r.push_back(i.one());
  return r;
}


ηsstream kvmmat_::kv_at(ηic &k) const
{
  if (!kv_has(k)) return ηesstream();
  if (kv_is_indirect(k))
  {
    V<ηsstream> ivs;
    for (let &i : kv_indirect(k)) ivs.push_back(ηisstream(db_->get(ikey(i))));
    return ηsstream_union(ivs);
  }
  else
    return ηisstream(kv_literal(k));
}

ηsstream kvmmat_::at(ηic &k) const
{
  let a = kv_at(k);
  let b = add_.find(k);
  let c = del_.find(k);
  return (a | (b != add_.end() ? ηsosstream(b->second) : ηesstream()))
       -      (c != del_.end() ? ηsosstream(c->second) : ηesstream());
}


}

#include "end.hh"
