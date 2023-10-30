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
  if (!kv_has(k) || !kv_ilen(k)) return;

  let iks = kv_ind_asc(k);
  let nk  = *iks.rbegin();      // largest (n k) pair
  let mi  = ubits(nk.y().i());  // max #indirects

  if (iks.size() <= mi) return;

  let nm = iks.size() - mi;          // number of merges required
  V<ηm>       mks; mks.reserve(nm);  // keys to merge
  V<ηsstream> mss; mss.reserve(nm);  // streams to merge
  V<ηm>       oks; oks.reserve(mi);  // other keys, preserved verbatim
  uN i = 0;
  for (let &x : iks)
    if (i++ <= nm) mks.push_back(x[1]), mss.push_back(ss_indirect1(x[1]));
    else           oks.push_back(x);

  let mn = ηsstream_union(mss)->all();
  oks.push_back(ηm{} << i64(mn.y().len()) << new_indirect(mn).all());
  ηm lv;
  lv << i64(oks.size());
  for (let &x : oks) lv << x;
  db_->set(lkey(k), lv);

  // Finally, delete the now-merged indirects
  for (let &i : mks) db_->del(ikey(i));
}


void kvmmat_::touch()
{
  // Not sophisticated, but it should basically work
  if (ss_ >= svo_ || iN(add_.size()) >= sko_ || iN(del_.size()) >= sko_)
    flush();
}

void kvmmat_::flush()
{
  // Invariant: elements from add_ and del_ are disjoint, but keys might not be.
  S<ηm> ks;
  for (let &[k, _] : add_) ks.insert(k);
  for (let &[k, _] : del_) ks.insert(k);
  for (let &k : ks) flush(k);
  db_->commit();
  A(!ss_, "kvmmat_::ss_ != 0: " << ss_);
}

void kvmmat_::flush(key &k)
{
  // Add staged values first, then delete; this way we get the rebalancing
  // in as necessary.
  if (staged_add(k))
  {
    add_kv(k, add_.at(k));
    for (let &x : add_.at(k)) ss_ -= x.lsize();
    add_.erase(k);
    balance(k);
  }

  if (staged_del(k))
  {
    del_kv(k, del_.at(k));
    for (let &x : del_.at(k)) ss_ -= x.lsize();
    del_.erase(k);
  }
}


void kvmmat_::stage_add(key &k, ηic &v)
{
  if (add_[k].insert(v).second) ss_ += v.lsize();
  unstage_del(k, v);
  touch();
}

void kvmmat_::stage_add(key &k, ηm &&v)
{
  if (add_[k].insert(mo(v)).second) ss_ += v.lsize();
  unstage_del(k, v);
  touch();
}

void kvmmat_::stage_del(key &k, ηic &v)
{
  if (del_[k].insert(v).second) ss_ += v.lsize();
  unstage_add(k, v);
  touch();
}

void kvmmat_::stage_del(key &k, ηm &&v)
{
  if (del_[k].insert(mo(v)).second) ss_ += v.lsize();
  unstage_add(k, v);
  touch();
}

void kvmmat_::unstage_add(key &k, ηic &v)
{
  let i = add_.find(k);
  if (i != add_.end())
  {
    if (i->second.erase(v)) ss_ -= v.lsize();
    if (i->second.empty())  add_.erase(i);
  }
}

void kvmmat_::unstage_del(key &k, ηic &v)
{
  let i = del_.find(k);
  if (i != del_.end())
  {
    if (i->second.erase(v)) ss_ -= v.lsize();
    if (i->second.empty())  del_.erase(i);
  }
}

bool kvmmat_::staged_add(key &k)
{
  // NOTE: should never be empty, but let's be sure
  return add_.contains(k) && !add_.at(k).empty();
}

bool kvmmat_::staged_del(key &k)
{
  return del_.contains(k) && !del_.at(k).empty();
}


ηsstream kvmmat_::ss(key &k) const
{
  return (ss_kv(k) | ss_add_stage(k)) - ss_del_stage(k);
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
  return !i.i() ? ss_literal(k) : ss_indirect(k);
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
  A(kv_ilen(k), "kv_ind_asc on non-indirect " << k);
  V<ηm> r; r.reserve(kv_ilen(k));
  for (let &x : db_->get(lkey(k)).next()) r.push_back(x.η()[1]);
  return r;
}

So<ηm> kvmmat_::kv_ind_asc(key &k) const
{
  A(kv_ilen(k), "kv_ind_asc on non-indirect " << k);
  So<ηm> r;
  for (let &x : db_->get(lkey(k)).next()) r.insert(x.η());
  return r;
}


ηm kvmmat_::new_indirect(stage const &v)
{
  uN s = 0;           for (let &x : v) s += x.lsize();
  ηm m; m.reserve(s); for (let &x : v) m << x.all();
  return new_indirect(m);
}

ηm kvmmat_::new_indirect(ηic &v)
{
  let k = genkey();
  db_->set(ikey(k), v);
  return k;
}

void kvmmat_::make_indirect(key &k)
{
  let ik = genkey();
  let v  = ηm{db_->get(lkey(k)).next()};
  let n  = v.y().len();
  db_->set(ikey(ik), v);
  db_->set(lkey(k),  ηm{} << 1 << (ηm{} << i64(n) << ik.all()));
}


void kvmmat_::add_kv_literal(key &k, stage const &v)
{
  A(!db_->has(lkey(k)), "add_kv_literal on existing key " << k);
  uN s = 0;                       for (let &x : v) s += x.lsize();
  ηm m; m.reserve(s + 2); m << 0; for (let &x : v) m << x.all();
  db_->set(lkey(k), m);
}

void kvmmat_::add_kv_indirect(key &k, stage const &v)
{
  let  ik = new_indirect(v);
  auto ks = kv_ind_asc(k); ks.insert(ηm{} << i64(v.size()) << ik.all());
  ηm   lv;
  lv << i64(ks.size());
  for (let &k : ks) lv << k;
  db_->set(lkey(k), lv);
}

void kvmmat_::add_kv(key &k, stage const &v)
{
  if      (!kv_has(k))                    add_kv_literal(k, v);
  else if (!kv_ilen(k)) make_indirect(k), add_kv_indirect(k, v);
  else                                    add_kv_indirect(k, v);
}


void kvmmat_::del_kv_literal(key &k, stage const &v)
{
  TODO("del_kv_literal");
}

void kvmmat_::del_kv_indirect(key &k, stage const &v)
{
  TODO("del_kv_indirect");
}

void kvmmat_::del_kv(key &k, stage const &v)
{
  TODO("del_kv");
}

void kvmmat_::del_all(key &k)
{
  TODO("del_all");
}


ηm kvmmat_::genkey()
{
  ++*(i64*)(nk_.data());
  while (db_->has(ikey(ηm{} << nk_)))
  {
    *(i64*)(nk_.data()) += now().time_since_epoch().count();
    nk_ = (isha2{} << (ηm{} << nk_))();
  }
  return ηm{} << nk_;
}


}

#include "end.hh"
