#include "pre-container-kv.hh"
#include "begin.hh"

namespace σ::pre
{


void kvmmat_::α(key &k, ηic &v, ξo)
{
  stage_add(k, v);
}

void kvmmat_::ω(key &k, ηic &v, ξo)
{
  if (v.empty()) del_all(k);
  else           stage_del(k, v);
}

void kvmmat_::ι(key &k, ηic&, ξo o)
{
  auto i = ss(k);
  while (*i) o.r() << **i, ++*i;
  o.r() << ηsig::τ;
}

void kvmmat_::κ(key &k, ηic&, ξo o)
{
  flush(k);
}

void kvmmat_::τ(ηic &x, ξo o)
{
  flush();
  o.r() << ηsig::τ;
}


void kvmmat_::balance(key &k)
{
  TODO("balance");
}


void kvmmat_::flush(key &k)
{
  TODO("flush");
}


void kvmmat_::flush()
{
  // Invariant: elements from add_ and del_ are disjoint, but keys might not be.
  S<ηm> ks;
  for (let &[k, _] : add_) ks.insert(k);
  for (let &[k, _] : del_) ks.insert(k);
  for (let &k : ks) flush(k);
}


void kvmmat_::stage_add(key &k, ηic &v)
{
  add_[k].insert(v);
  unstage_del(k, v);
}

void kvmmat_::stage_add(key &k, ηm &&v)
{
  add_[k].insert(mo(v));
  unstage_del(k, v);
}

void kvmmat_::stage_del(key &k, ηic &v)
{
  del_[k].insert(v);
  unstage_add(k, v);
}

void kvmmat_::stage_del(key &k, ηm &&v)
{
  del_[k].insert(mo(v));
  unstage_add(k, v);
}

void kvmmat_::unstage_add(key &k, ηic &v)
{
  let i = add_.find(k);
  if (i != add_.end())
  {
    let j = i->second.find(v);
    if (j != i->second.end()) i->second.erase(j);
    if (i->second.empty()) add_.erase(i);
  }
}

void kvmmat_::unstage_del(key &k, ηic &v)
{
  let i = del_.find(k);
  if (i != del_.end())
  {
    let j = i->second.find(v);
    if (j != i->second.end()) i->second.erase(j);
    if (i->second.empty()) add_.erase(i);
  }
}


ηsstream kvmmat_::ss_add_stage(key &k) const
{
  let i = add_.find(k);
  return i != add_.end() ? ηsosstream(i->second) : ηesstream();
}

ηsstream kvmmat_::ss_del_stage(key &k) const
{
  let i = del_.find(k);
  return i != del_.end() ? ηsosstream(i->second) : ηesstream();
}

ηsstream kvmmat_::ss_kv(key &k) const
{
  if (!db_->has(lkey(k))) return ηesstream();
  let i = db_->get(lkey(k));
  return i.i() ? ss_literal(k) : ss_indirect(k);
}

ηsstream kvmmat_::ss_literal(key &k) const
{
  return ηisstream(db_->get(lkey(k)).next());
}

ηsstream kvmmat_::ss_indirect(key &k) const
{
  V<ηsstream> ss;
  for (let &k : kv_indirects(k)) ss.push_back(ss_indirect1(k));
  return ηsstream_union(ss);
}

ηsstream kvmmat_::ss_indirect1(key &k) const
{
  return ηisstream(db_->get(ikey(k)));
}


bool kvmmat_::kv_has(key &k) const
{
  return db_->has(lkey(k));
}

uN kvmmat_::kv_ilen(key &k) const
{
  return db_->get(lkey(k)).i();
}

V<ηm> kvmmat_::kv_indirects(key &k) const
{
  V<ηm> r; r.reserve(kv_ilen(k));
  for (let &x : db_->get(lkey(k))) r.push_back(x[1]);
  return r;
}

So<ηm> kvmmat_::kv_ind_asc(key &k) const
{
  So<ηm> r;
  for (let &x : db_->get(lkey(k))) r.insert(x);
  return r;
}


kvmmat_::ind kvmmat_::new_indirect(stage const &v)
{
  let k = genkey();
  uN s = 0;           for (let &x : v) s += x.lsize();
  ηm m; m.reserve(s); for (let &x : v) m << x;
  db_->set(k, m);
  return k;
}


void kvmmat_::add_kv_literal(key &k, stage const &v)
{
  A(!db_->has(lkey(k)), "add_kv_literal on existing key " << k);
  uN s = 0;                       for (let &x : v) s += x.lsize();
  ηm m; m.reserve(s + 2); m << 0; for (let &x : v) m << x;
  db_->set(lkey(k), m);
}


}

#include "end.hh"
