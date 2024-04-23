#include <sys/mman.h>
#include <sys/stat.h>

#include "lmdb.hh"
#include "../begin.hh"

namespace σ
{

using namespace τ;


void Γlmdb(Γφ &g)
{
  g.def_p1("@|L", [](φig, Stc &f, φig, Stc &t, ψ q, ξo o)
    {
      lmdb db{q.t(), f, t};

      Mp<MDB_txn> r(mdb_txn_abort);
      MDB_dbi     dbi;
      int         rc;

      A((rc = mdb_txn_begin(db.env().get(), nullptr, MDB_RDONLY, &r)) == MDB_SUCCESS,
        "r::mdb_txn_begin() failed: " << mdb_strerror(rc));
      A((rc = mdb_dbi_open(r, t.c_str(), MDB_CREATE, &dbi)) == MDB_SUCCESS,
        "r::mdb_dbi_open() failed: " << mdb_strerror(rc));

      Mp<MDB_cursor> c(mdb_cursor_close);
      A((rc = mdb_cursor_open(r, dbi, &c)) == MDB_SUCCESS,
        "mdb_cursor_open() failed: " << mdb_strerror(rc));
      MDB_val k, v;
      while ((rc = mdb_cursor_get(c, &k, &v, MDB_NEXT)) == MDB_SUCCESS)
        o.r(k.mv_size + v.mv_size + 8)
          << ηi{Rc<u8c*>(k.mv_data), k.mv_size}
          << Sn<u8c>{Rc<u8c*>(v.mv_data), v.mv_size};
      A(rc == MDB_NOTFOUND, "mdb_cursor_get() failed: " << mdb_strerror(rc));
    });
}


static uN filesize(Stc &f)
{
  std::ifstream in(f, std::ifstream::ate | std::ifstream::binary);
  if (!in.is_open()) return 0;
  return in.tellg();
}


lmdb::lmdb(τe &te, Stc &f, Stc &t, uN mapsize, uN maxdbs, uN mss, f64 rf)
  : te_(te), f_(f), t_(t), ss_(0), mss_(mss),
    dsize_(0), isize_(0), next_rep_(disk_size() * rf), rep_factor_(rf),
    prof_get_outer_(measurement_for(ηm{} << "lmdb" << f << t << "get_outer")),
    prof_get_inner_(measurement_for(ηm{} << "lmdb" << f << t << "get_inner")),
    prof_has_outer_(measurement_for(ηm{} << "lmdb" << f << t << "has_outer")),
    prof_has_inner_(measurement_for(ηm{} << "lmdb" << f << t << "has_inner")),
    prof_prefetch_outer_(measurement_for(ηm{} << "lmdb" << f << t << "prefetch_outer")),
    prof_prefetch_inner_(measurement_for(ηm{} << "lmdb" << f << t << "prefetch_inner")),
    prof_del_staged_(measurement_for(ηm{} << "lmdb" << f << t << "del_staged")),
    prof_set_staged_(measurement_for(ηm{} << "lmdb" << f << t << "set_staged")),
    prof_commit_outer_(measurement_for(ηm{} << "lmdb" << f << t << "commit_outer")),
    prof_commit_write_(measurement_for(ηm{} << "lmdb" << f << t << "commit_write")),
    prof_reader_(measurement_for(ηm{} << "lmdb" << f << t << "reader")),
    prof_repack_outer_(measurement_for(ηm{} << "lmdb" << f << t << "repack_outer")),
    prof_repack_inner_(measurement_for(ηm{} << "lmdb" << f << t << "repack_inner"))
{
  int rc;

  uN m = std::max(filesize(f) * 4ull, mapsize * 1ull);

  MDB_env *e;
  A((rc = mdb_env_create(&e))            == MDB_SUCCESS, "mdb_env_create() failed: "      << mdb_strerror(rc));
  A((rc = mdb_env_set_maxdbs(e, maxdbs)) == MDB_SUCCESS, "mdb_env_set_maxdbs() failed: "  << mdb_strerror(rc));
  A((rc = mdb_env_set_mapsize(e, m))     == MDB_SUCCESS, "mdb_env_set_mapsize() failed: " << mdb_strerror(rc));
  A((rc = mdb_env_open(e, f.c_str(), MDB_NOSUBDIR | MDB_NOSYNC | MDB_NOMETASYNC | MDB_NORDAHEAD | MDB_NOTLS, 0664)) == MDB_SUCCESS,
    "mdb_env_open(" << f << ") failed: " << mdb_strerror(rc));

  MDB_txn *dbi_t;
  A((rc = mdb_txn_begin(e, nullptr, 0, &dbi_t)) == MDB_SUCCESS,
    "mdb_txn_begin() failed: " << mdb_strerror(rc));
  A((rc = mdb_dbi_open(dbi_t, t.c_str(), MDB_CREATE, &d_)) == MDB_SUCCESS,
    "mdb_dbi_open(\"" << t << "\") failed: " << mdb_strerror(rc));
  mdb_txn_commit(dbi_t);

  e_ = Sp<MDB_env>{e, mdb_env_close};
}


lmdb::~lmdb()
{
  commit();
  mdb_env_close(e_.get());
}


lmdb::v<ηi> lmdb::get(ηmc &k) const
{
  // NOTE: must hold sl through reader() call to guarantee consistent data
  let     t1 = prof_get_outer_.start();
  Sl<Smu> sl{smu_};
  if (dstage_.contains(k)) return {};
  if (let i = istage_.find(k); i != istage_.end())
    return {*i->second, {}, i->second};

  let     t2 = prof_get_inner_.start();
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
  let t1 = prof_has_outer_.start();
  Sl<Smu> sl{smu_};
  if (dstage_.contains(k))                         return false;
  if (let i = istage_.find(k); i != istage_.end()) return true;

  let     t2 = prof_has_inner_.start();
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
    let t = prof_del_staged_.start();
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
    let t = prof_set_staged_.start();
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


void lmdb::prefetch(ηmc &k) const
{
  // NOTE: must hold sl through reader() call to guarantee consistent data
  let t1 = prof_prefetch_outer_.start();
  Sl<Smu> sl{smu_};
  if (dstage_.contains(k))                         return;
  if (let i = istage_.find(k); i != istage_.end()) return;

  let     t2 = prof_prefetch_inner_.start();
  let     r  = reader();
  MDB_val mk = val(k);
  MDB_val mv;
  let rc = mdb_get(r->t, d_, &mk, &mv);
  if (rc != MDB_SUCCESS) return;  // errors are fine here; prefetch will be nop
  madvise(mv.mv_data, mv.mv_size, MADV_WILLNEED);
}


void lmdb::commit(bool sync)
{
  let t = prof_commit_outer_.start();

  // NOTE: cl locks out all modification operators for the duration of this
  // commit, which is critical because we switch from non-blocking to blocking
  // when we drop sl below.
  {
    Ul<Smu> cl{cmu_};
    commit_(sync);
  }
  maybe_repack(sync);
}


void lmdb::maybe_repack(bool sync)
{
  if (rep_factor_ == 0) return;

  {
    Sl<Smu> cl{cmu_};
    if (dsize_ + isize_ < next_rep_) return;
  }
  repack(sync);
}


uS lmdb::disk_size() const
{
  struct stat st;
  if (stat(f_.c_str(), &st)) return 16384;  // some default size
  return st.st_size;
}


void lmdb::commit_(bool sync)
{
  // NOTE: this function must be called with cmu_ unique-locked.

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
    let t = prof_commit_write_.start();
    Sl<Smu> sl{smu_};

    MDB_txn *w;
    int rc = mdb_txn_begin(e_.get(), nullptr, 0, &w);
    A(rc == MDB_SUCCESS, "mdb_txn_begin() failed: " << mdb_strerror(rc));

    // Deletions come first because they can free space that will be used by
    // insertions.
    for (let &k : dstage_)
    {
      MDB_val mk = val(k);
      MDB_val mv;
      if (mdb_get(w, d_, &mk, &mv) == MDB_SUCCESS) dsize_ += mv.mv_size;
      A((rc = mdb_del(w, d_, &mk, nullptr)) == MDB_SUCCESS || rc == MDB_NOTFOUND,
        "mdb_del() failed: " << mdb_strerror(rc));
    }

    // Now handle insertions/updates
    for (let &[k, v] : istage_)
    {
      // Assume we're allocating new space for every written value, since a
      // reader might refer to any value we have.
      isize_ += v->lsize();

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


void lmdb::repack(bool sync)
{
  let t = prof_repack_outer_.start();
  Ul<Smu> cl{cmu_};

  // First clear the stage. This will minimize the amount of data that
  // accumulates as we perform the repack.
  commit_(sync);

  let t1 = prof_repack_inner_.start();
  let repack_f = f_ + ".repack";

  // Open a new environment and copy data over to it. This will replace the
  // current env. We start by copying parameters from the current environment,
  // then iterating through the keys.
  //
  // Nobody can commit while we're doing this because we're holding cmu_; if the
  // stage fills up, the writer will block until we're done.

  int rc;
  MDB_envinfo ei;
  A((rc = mdb_env_info(e_.get(), &ei)) == MDB_SUCCESS,
    "lmdb::repack mdb_env_info() failed: " << mdb_strerror(rc));

  MDB_env *ne;
  A((rc = mdb_env_create(&ne)) == MDB_SUCCESS,
    "lmdb::repack mdb_env_create() failed: " << mdb_strerror(rc));

  A((rc = mdb_env_set_mapsize(ne, ei.me_mapsize)) == MDB_SUCCESS,
    "lmdb::repack mdb_env_set_mapsize() failed: " << mdb_strerror(rc));

  unsigned flags;
  A((rc = mdb_env_get_flags(e_.get(), &flags)) == MDB_SUCCESS,
    "lmdb::repack mdb_env_get_flags() failed: " << mdb_strerror(rc));

  A((rc = mdb_env_open(ne, repack_f.c_str(), flags, 0664)) == MDB_SUCCESS,
    "lmdb::repack mdb_env_open() failed: " << mdb_strerror(rc));

  // Copy data; to do this, we first need to create the new database.
  MDB_txn *r;
  A((rc = mdb_txn_begin(e_.get(), nullptr, MDB_RDONLY, &r)) == MDB_SUCCESS,
    "lmdb::repack mdb_txn_begin() for reader failed: " << mdb_strerror(rc));

  MDB_txn *w;
  A((rc = mdb_txn_begin(ne, nullptr, 0, &w)) == MDB_SUCCESS,
    "lmdb::repack mdb_txn_begin() for writer failed: " << mdb_strerror(rc));

  MDB_dbi d;
  A((rc = mdb_dbi_open(w, t_.c_str(), MDB_CREATE, &d)) == MDB_SUCCESS,
    "lmdb::repack mdb_dbi_open() for writer failed: " << mdb_strerror(rc));

  MDB_cursor *c;
  A((rc = mdb_cursor_open(r, d_, &c)) == MDB_SUCCESS,
    "lmdb::repack mdb_cursor_open() for reader failed: " << mdb_strerror(rc));

  MDB_val mk, mv;
  while ((rc = mdb_cursor_get(c, &mk, &mv, MDB_NEXT)) == MDB_SUCCESS)
    A((rc = mdb_put(w, d, &mk, &mv, 0)) == MDB_SUCCESS,
      "lmdb::repack mdb_put() failed: " << mdb_strerror(rc));

  A(rc == MDB_NOTFOUND, "lmdb::repack mdb_cursor_get() failed: " << mdb_strerror(rc));

  mdb_txn_abort(r);
  mdb_cursor_close(c);
  mdb_txn_commit(w);

  // Drop the current environment reference and replace it with the new one. The
  // old environment will be closed as soon as nobody is using any values from
  // it.
  e_ = Sp<MDB_env>{ne, mdb_env_close};
  d_ = d;

  // The stage will now apply to the new environment. We don't need to commit
  // here, although we could if we wanted to.

  A((rc = rename(repack_f.c_str(), f_.c_str())) == 0,
    "lmdb::repack rename() failed: " << strerror(errno));

  next_rep_ = disk_size() * rep_factor_;
  isize_ = dsize_ = 0;
}


void lmdb::maybe_commit(bool sync)
{
  {
    Sl<Smu> l{smu_};
    if (ss_ < mss_) return;
  }
  commit(sync);
}


Sp<lmdb::rtx_> lmdb::reader() const
{
  let t = prof_reader_.start();

  // NOTE: this function is always called from a context with a shared stage
  // lock (which matters due to its potential interaction with commit())
  {
    Sl<Smu> l{rmu_};
    if (rt_.get() != nullptr) return rt_;
  }

  Ul<Smu> l{rmu_};
  return rt_ = Sp<rtx_>{new rtx_{e_}};
}


lmdb::rtx_::rtx_(Sp<MDB_env> e) : e(e)
{
  int rc;
  A((rc = mdb_txn_begin(e.get(), nullptr, MDB_RDONLY, &t)) == MDB_SUCCESS,
    "lmdb::rtx_: mdb_txn_begin failed: " << mdb_strerror(rc));
}


lmdb::rtx_::~rtx_()
{
  mdb_txn_abort(t);
}


}

#include "../end.hh"
