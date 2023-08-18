#include "pre-db.hh"
#include "../begin.hh"

namespace σ::pre
{


struct kvsat_ : public virtual at_
{
  kvsat_(cback const &l, Sp<kv_> db) : at_(ct_set{}, l), db(db) {}

  void α(ηic &x, ηic&, ξo)   override { db->set(x, ηic{m, 1}); }
  void ω(ηic &x, ηic&, ξo)   override { db->del(x); }
  void ι(ηic &x, ηic&, ξo o) override { o.r(x.lsize() + 2) << x.all() << db->has(x); }
  void τ(ηic &x, ξo o)       override { db->sync(); o.r() << ηsig::τ; }

  Sp<kv_> db;

  static constexpr u8 m[] = {0x11, 0};
};


struct kvmat_ : public virtual at_
{
  kvmat_(cback const &l, Sp<kv_> db) : at_(ct_map{}, l), db(db) {}
  void α(ηic &k, ηic &v, ξo) override { db->set(k, v); }
  void ω(ηic &k, ηic&, ξo)   override { db->del(k); }
  void ι(ηic &k, ηic&, ξo o) override
    { let r = db->get(k);
      if (!r.empty()) o.r(k.lsize() + r.size() + 8) << k.all() << r.all();
      else            o.r(k.lsize() + 2)            << k.all() << ηsig::ω; }
  void τ(ηic &x, ξo o) override { db->sync(); o.r() << ηsig::τ; }

  Sp<kv_> db;
};


struct lmdb_ls_ : public virtual at_ls_
{
  lmdb_ls_(cb_lmdb const &l, Sp<lmdb_db> mdb) : at_ls_(l), mdb(mdb), db(l.db) {}

  void ls(ξo o)
    { Mp<MDB_txn> r(mdb_txn_abort);
      MDB_dbi     dbi;
      int         rc;

      A((rc = mdb_txn_begin(mdb->e, nullptr, MDB_RDONLY, &r)) == MDB_SUCCESS,
        "r::mdb_txn_begin() failed: " << mdb_strerror(rc));
      A((rc = mdb_dbi_open(r, db.c_str(), MDB_CREATE, &dbi)) == MDB_SUCCESS,
        "r::mdb_dbi_open() failed: " << mdb_strerror(rc));

      Mp<MDB_cursor> c(mdb_cursor_close);
      A((rc = mdb_cursor_open(r, dbi, &c)) == MDB_SUCCESS,
        "mdb_cursor_open() failed: " << mdb_strerror(rc));
      MDB_val k, v;
      while ((rc = mdb_cursor_get(c.get(), &k, &v, MDB_NEXT)) == MDB_SUCCESS)
        o.r(k.mv_size + v.mv_size + 8)
          << Sn<u8c>{Rc<u8c*>(k.mv_data), k.mv_size}
          << Sn<u8c>{Rc<u8c*>(v.mv_data), v.mv_size};
      A(rc == MDB_NOTFOUND, "mdb_cursor_get() failed: " << mdb_strerror(rc)); }

  Sp<lmdb_db> mdb;
  St          db;
};


Sp<at_> lmdb_set(cb_lmdb const &l)
{
  return Sp<at_>(new kvsat_(l, kv_lmdb(l.f, l.db)));
}

Sp<at_> lmdb_map(cb_lmdb const &l)
{
  return Sp<at_>(new kvmat_(l, kv_lmdb(l.f, l.db)));
}


Sp<at_ls_> lmdb_ls(cb_lmdb const &l)
{
  return Sp<at_ls_>(new lmdb_ls_(l, lmdb_open(l.f)));
}


}

#include "../end.hh"
