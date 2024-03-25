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

      A((rc = mdb_txn_begin(db.env(), nullptr, MDB_RDONLY, &r)) == MDB_SUCCESS,
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


lmdb::lmdb(τe &te, Stc &f, Stc &t, uN mapsize, uN maxdbs, uN mss)
  : te_(te), f_(f), t_(t), ss_(0), mss_(mss),
    prof_get_outer_(measurement_for(ηm{} << "lmdb" << f << t << "get_outer")),
    prof_get_inner_(measurement_for(ηm{} << "lmdb" << f << t << "get_inner")),
    prof_has_outer_(measurement_for(ηm{} << "lmdb" << f << t << "has_outer")),
    prof_has_inner_(measurement_for(ηm{} << "lmdb" << f << t << "has_inner")),
    prof_del_staged_(measurement_for(ηm{} << "lmdb" << f << t << "del_staged")),
    prof_set_staged_(measurement_for(ηm{} << "lmdb" << f << t << "set_staged")),
    prof_commit_outer_(measurement_for(ηm{} << "lmdb" << f << t << "commit_outer")),
    prof_commit_write_(measurement_for(ηm{} << "lmdb" << f << t << "commit_write")),
    prof_reader_(measurement_for(ηm{} << "lmdb" << f << t << "reader"))
{
  int rc;

  on_sig_ = Sp<F<void(int)>>{new F<void(int)>{[this](int) { commit(true); }}};
  te_.sig_register(on_sig_);

  uN m = std::max(filesize(f) * 4ull, mapsize * 1ull);

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
  if (τe_() != nullptr) te_.sig_unregister(on_sig_);
  commit();
  mdb_env_close(e_);
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


void lmdb::commit(bool sync)
{
  let t = prof_commit_outer_.start();

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
    let t = prof_commit_write_.start();
    Sl<Smu> sl{smu_};

    MDB_txn *w;
    int rc = mdb_txn_begin(e_, nullptr, 0, &w);
    A(rc == MDB_SUCCESS, "mdb_txn_begin() failed: " << mdb_strerror(rc));

    // Deletions come first because they can free space that will be used by
    // insertions.
    for (let &k : dstage_)
    {
      MDB_val mk = val(k);
      A((rc = mdb_del(w, d_, &mk, nullptr)) == MDB_SUCCESS || rc == MDB_NOTFOUND,
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
