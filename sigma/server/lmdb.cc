#include "lmdb.hh"
#include "../begin.hh"

namespace σ
{

using namespace τ;


static uN filesize(Stc &f)
{
  std::ifstream in(f, std::ifstream::ate | std::ifstream::binary);
  if (!in.is_open()) return 0;
  return in.tellg();
}


lmdb::lmdb(Stc &f, Stc &t, uN mapsize, uN maxdbs, uN mss) : mss_(mss)
{
  int rc;

  uN m = std::max(filesize(f) * 2ull, mapsize * 1ull);

  A((rc = mdb_env_create(&e_))            == MDB_SUCCESS, "mdb_env_create() failed: "      << mdb_strerror(rc));
  A((rc = mdb_env_set_maxdbs(e_, maxdbs)) == MDB_SUCCESS, "mdb_env_set_maxdbs() failed: "  << mdb_strerror(rc));
  A((rc = mdb_env_set_mapsize(e_, m))     == MDB_SUCCESS, "mdb_env_set_mapsize() failed: " << mdb_strerror(rc));
  A((rc = mdb_env_open(e_, f.c_str(), MDB_NOSUBDIR | MDB_NOSYNC | MDB_NOMETASYNC | MDB_NORDAHEAD | MDB_NOTLS, 0664)) == MDB_SUCCESS,
    "mdb_env_open(" << f << ") failed: " << mdb_strerror(rc));

  MDB_txn *dbi_t;
  A((rc = mdb_txn_begin(e_, nullptr, 0, &dbi_t)) == MDB_SUCCESS,
    "mdb_txn_begin() failed: " << mdb_strerror(rc));
  A((rc = mdb_dbi_open(dbi_t, t.c_str(), MDB_CREATE, &d_)) == MDB_SUCCESS,
    "mdb_dbi_open(\"" << t << "\") failed: " << mdb_strerror(rc));
  mdb_txn_commit(dbi_t);
}


lmdb::~lmdb()
{
  commit();
  mdb_env_close(e_);
}


lmdb::v<ηi> lmdb::get(ηmc &k) const
{
  // NOTE: must hold sl through reader() call to guarantee consistent data
  Sl<Smu> sl{smu_};
  if (dstage_.contains(k)) return {};
  if (let i = istage_.find(k); i != istage_.end())
    return {*i->second, {}, i->second};

  let     r  = reader();
  MDB_val mk = val(k);
  MDB_val mv;
  let rc = mdb_get(r->t, d_, &mk, &mv);
  if (rc == MDB_NOTFOUND) return {};
  A(rc == MDB_SUCCESS, "mdb_get() failed: " << mdb_strerror(rc));
  return {{(u8c*) mv.mv_data, mv.mv_size}, r, {}};
}


bool lmdb::has(ηmc &k) const
{
  // NOTE: must hold sl through reader() call to guarantee consistent data
  Sl<Smu> sl{smu_};
  if (dstage_.contains(k))                         return false;
  if (let i = istage_.find(k); i != istage_.end()) return true;

  let     r  = reader();
  MDB_val mk = val(k);
  MDB_val mv;
  let rc = mdb_get(r->t, d_, &mk, &mv);
  if (rc == MDB_NOTFOUND) return false;
  A(rc == MDB_SUCCESS, "mdb_get() failed: " << mdb_strerror(rc));
  return true;
}


void lmdb::del(ηmc &k)
{
  {
    Sl<Smu> cl{cmu_};  // IMPORTANT: cannot modify stage during commit
    Ul<Smu> sl{smu_};
    istage_.erase(k);  // no updates to this key are relevant anymore
    dstage_.insert(k);
    ss_ += k.lsize();
  }
  maybe_commit();
}


void lmdb::set(ηmc &k, ηm &&v)
{
  let vp = Sp<ηm>{new ηm{mo(v)}};
  {
    Sl<Smu> cl{cmu_};  // IMPORTANT: cannot modify stage during commit
    Ul<Smu> sl{smu_};
    dstage_.erase(k);
    istage_[k] = vp;
    ss_ += k.lsize() + vp->lsize();
  }
  maybe_commit();
}


void lmdb::set(ηmc &k, ηic &v)
{
  set(k, ηm{v});
}


void lmdb::commit(bool sync)
{
  // NOTE: cl locks out all modification operators for the duration of this
  // commit, which is critical because we switch from non-blocking to blocking
  // when we drop sl below.
  Ul<Smu> cl{cmu_};

  // Lose the default reference to the current read transaction: best case we
  // close it, otherwise we prepare to open a new one with updated data. Note
  // that we don't block readers at this point, so they may create a new
  // transaction while we're committing data. That's fine; we'll release the
  // read transaction reference again after committing everything.
  {
    Ul<Smu> rl{rmu_};
    rt_ = {};
  }

  {
    Sl<Smu> sl{smu_};

    // First things first: figure out how much data we're committing and resize
    // the map if necessary. This must be done before we create the write
    // transaction. I'm adding 64 bytes of overhead per thing here because
    // that's a reasonable upper bound on LMDB overhead per key (I measured 38
    // bytes on 1M keys).
    uN us = 0;
    for (let &k       : dstage_) us += k.lsize() + 64;
    for (let &[k, pv] : istage_) us += k.lsize() + pv->lsize() + 64;
    while (should_resize(us))
    {
      MDB_envinfo i;
      int rc = mdb_env_info(e_, &i);
      A(rc == MDB_SUCCESS, "mdb_env_info() failed: " << mdb_strerror(rc));
      rc = mdb_env_set_mapsize(e_, i.me_mapsize * 2);
      A(rc == MDB_SUCCESS, "mdb_env_set_mapsize() failed: " << mdb_strerror(rc));
    }

    // Now we're safe to create the write transaction and commit the stage.
    MDB_txn *w;
    int rc = mdb_txn_begin(e_, nullptr, 0, &w);
    A(rc == MDB_SUCCESS, "mdb_txn_begin() failed: " << mdb_strerror(rc));

    // Deletions come first because they can free space that will be used by
    // insertions.
    for (let &k : dstage_)
    {
      MDB_val mk = val(k);
      A((rc = mdb_del(w, d_, &mk, nullptr)) == MDB_SUCCESS,
        "mdb_del() failed: " << mdb_strerror(rc));
    }

    // Now handle insertions/updates
    for (let &[k, v] : istage_)
    {
      MDB_val mk = val(k);
      MDB_val mv = val(*v);
      A((rc = mdb_put(w, d_, &mk, &mv, 0)) == MDB_SUCCESS,
        "mdb_put() failed: " << mdb_strerror(rc));
    }

    A((rc = mdb_txn_commit(w)) == MDB_SUCCESS,
      "mdb_txn_commit() failed: " << mdb_strerror(rc));

    // NOTE: it's safe to drop sl here because we're protected by cl.
  }

  {
    // Block readers while we delete the stage and erase the current read
    // transaction, causing any future reads to pull the value from LMDB instead
    // of the stage.
    Ul<Smu> sl{smu_};
    Ul<Smu> rl{rmu_};
    dstage_.clear();
    istage_.clear();
    rt_ = {};
  }
}


bool lmdb::should_resize(uN n, f64 safety) const
{
  MDB_stat    s;
  MDB_envinfo i;
  if (mdb_env_stat(e_, &s) != MDB_SUCCESS || mdb_env_info(e_, &i) != MDB_SUCCESS)
    return true;

  let t =     s.ms_psize * (uS) i.me_mapsize / s.ms_psize;
  let f = t - s.ms_psize * (uS) i.me_last_pgno * safety;
  return n >= f;
}


void lmdb::maybe_commit()
{
  {
    Sl<Smu> l{smu_};
    if (ss_ < mss_) return;
  }
  commit();
}


Sp<lmdb::rtx_> lmdb::reader() const
{
  // NOTE: this function is always called from a context with a shared stage
  // lock (which matters due to its potential interaction with commit())
  {
    Sl<Smu> l{rmu_};
    if (rt_.get() != nullptr) return rt_;
  }

  Ul<Smu> l{rmu_};
  return rt_ = Sp<rtx_>{new rtx_{e_}};
}


lmdb::rtx_::rtx_(MDB_env *e)
{
  int rc;
  A((rc = mdb_txn_begin(e, nullptr, MDB_RDONLY, &t)) == MDB_SUCCESS,
    "lmdb::rtx_: mdb_txn_begin failed: " << mdb_strerror(rc));
}


lmdb::rtx_::~rtx_()
{
  mdb_txn_abort(t);
}


}

#include "../end.hh"
