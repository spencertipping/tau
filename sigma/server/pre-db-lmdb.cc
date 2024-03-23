#include <fstream>
#include <lmdb.h>

#include "pre-db.hh"

#include "../../tau.hh"
#include "../../tau/begin.hh"

namespace σ::pre
{


static uN filesize(Stc &f)
{
  std::ifstream in(f, std::ifstream::ate | std::ifstream::binary);
  if (!in.is_open()) return 0;
  return in.tellg();
}


lmdb_db::lmdb_db(Stc &f, uN max_dbs, uN mapsize)
{
  int rc;

  uN m = std::max(filesize(f) * 2ull, mapsize * 1ull);

  A((rc = mdb_env_create(&e_))             == MDB_SUCCESS, "mdb_env_create() failed: "      << mdb_strerror(rc));
  A((rc = mdb_env_set_maxdbs(e_, max_dbs)) == MDB_SUCCESS, "mdb_env_set_maxdbs() failed: "  << mdb_strerror(rc));
  A((rc = mdb_env_set_mapsize(e_, m))      == MDB_SUCCESS, "mdb_env_set_mapsize() failed: " << mdb_strerror(rc));
  A((rc = mdb_env_open(e_, f.c_str(), MDB_NOSUBDIR | MDB_NOSYNC | MDB_NOMETASYNC | MDB_NORDAHEAD | MDB_NOTLS, 0664)) == MDB_SUCCESS,
    "mdb_env_open(" << f << ") failed: " << mdb_strerror(rc));
}


lmdb_db::~lmdb_db()
{
  commit();
  close_readers();
  mdb_env_close(e_);
}


bool lmdb_db::should_resize(uN n, f64 safety) const
{
  MDB_stat    s;
  MDB_envinfo i;
  if (mdb_env_stat(e_, &s) != MDB_SUCCESS || mdb_env_info(e_, &i) != MDB_SUCCESS)
    return true;

  let t =     s.ms_psize * (uS) i.me_mapsize / s.ms_psize;
  let f = t - s.ms_psize * (uS) i.me_last_pgno * safety;
  return n >= f;
}


void lmdb_db::reserve(uN n)
{
  while (should_resize(us_ + n))
  {
    Ul<Smu> l1{rm_};
    Ul<Rmu> l2{wm_};
    reset();

    MDB_envinfo i;
    int rc = mdb_env_info(e_, &i);
    A(rc == MDB_SUCCESS, "mdb_env_info() failed: " << mdb_strerror(rc));
    rc = mdb_env_set_mapsize(e_, i.me_mapsize * 2);
    A(rc == MDB_SUCCESS, "mdb_env_set_mapsize() failed: " << mdb_strerror(rc));
  }

  us_ += n;
}


void lmdb_db::sync()
{
  commit();
  mdb_env_sync(e_, 1);
}


void lmdb_db::commit()
{
  Ul<Rmu> l{wm_};
  if (w_ != nullptr)
  {
    int rc = mdb_txn_commit(w_);
    A(rc == MDB_SUCCESS, "mdb_txn_commit() failed: " << mdb_strerror(rc));
    us_ = 0;
    w_  = nullptr;
  }
}


ηi lmdb_db::get(Stc &db, ηic &k)
{
  MDB_val key = {k.lsize(), (void *) k.ldata()};
  MDB_val val;
  int rc = mdb_get(rt(), open(db), &key, &val);
  if (rc == MDB_NOTFOUND) return ηi{};
  A(rc == MDB_SUCCESS, "mdb_get() failed: " << mdb_strerror(rc));
  return ηi{(u8c*) val.mv_data, val.mv_size};
}


bool lmdb_db::has(Stc &db, ηic &k)
{
  MDB_val key = {k.lsize(), (void *) k.ldata()};
  MDB_val val;
  int rc = mdb_get(rt(), open(db), &key, &val);
  if (rc == MDB_NOTFOUND) return false;
  A(rc == MDB_SUCCESS, "mdb_get() failed: " << mdb_strerror(rc));
  return true;
}


void lmdb_db::del(Stc &db, ηic &k)
{
  Ul<Rmu> l{wm_};
  MDB_val key = {k.lsize(), (void *) k.ldata()};
  reserve(key.mv_size + 64);

  let rc = mdb_del(wt(), open(db), &key, nullptr);
  A(rc == MDB_SUCCESS, "mdb_del() failed: " << mdb_strerror(rc));
  ++re_;  // notify all readers that they need a new transaction
}


void lmdb_db::set(Stc &db, ηic &k, ηic &v)
{
  Ul<Rmu> l{wm_};
  MDB_val key = {k.lsize(), (void *) k.ldata()};
  MDB_val val = {v.lsize(), (void *) v.ldata()};
  reserve(key.mv_size + val.mv_size);

  let rc = mdb_put(wt(), open(db), &key, &val, 0);
  A(rc == MDB_SUCCESS, "mdb_put() failed: " << mdb_strerror(rc));
  ++re_;  // notify all readers that they need a new transaction
}


void lmdb_db::reset()
{
  Ul<Rmu> l{wm_};
  commit();
  close_readers();
}


void lmdb_db::close_readers()
{
  for (let &i : r_) mdb_txn_abort(i.second.t);
  r_.clear();
}


MDB_txn *lmdb_db::rt()
{
  int rc;
  let tid = std::this_thread::get_id();
  reader r{~uN(0), nullptr};

  {
    Sl<Smu> l{rm_};
    let i = r_.find(tid);
    if (i != r_.end()) r = i->second;
  }

  if (r.rev != re_ || r.t == nullptr)
  {
    Ul<Smu> l{rm_};
    if (r.rev != re_) commit();  // make outstanding writes visible

    if (r.t != nullptr)
    {
      mdb_txn_reset(r.t);
      rc = mdb_txn_renew(r.t);
      A(rc == MDB_SUCCESS, "mdb_txn_renew() failed: " << mdb_strerror(rc));
    }
    else
    {
      rc = mdb_txn_begin(e_, nullptr, MDB_RDONLY, &r.t);
      A(rc == MDB_SUCCESS, "mdb_txn_begin() failed: " << mdb_strerror(rc));
    }

    r.rev = re_;
    r_[tid] = r;
  }

  return r.t;
}


MDB_txn *lmdb_db::wt()
{
  Ul<Rmu> l{wm_};
  if (w_ == nullptr)
  {
    close_readers();
    MDB_envinfo i;
    mdb_env_info(e_, &i);
    A(i.me_numreaders == 0,
      "me_numreaders = " << i.me_numreaders << " (should be 0)");

    let rc = mdb_txn_begin(e_, nullptr, 0, &w_);
    A(rc == MDB_SUCCESS, "mdb_txn_begin() failed: " << mdb_strerror(rc));
  }
  return w_;
}


MDB_dbi lmdb_db::open(Stc &db)
{
  {
    Sl<Smu> l{dm_};
    let i = d_.find(db);
    if (i != d_.end()) return i->second;
  }

  Ul<Smu> l{dm_};
  let rc = mdb_dbi_open(wt(), db.c_str(), MDB_CREATE, &d_[db]);
  A(rc == MDB_SUCCESS, "mdb_dbi_open() failed: " << mdb_strerror(rc));
  return d_[db];
}


}

#include "../../tau/end.hh"
