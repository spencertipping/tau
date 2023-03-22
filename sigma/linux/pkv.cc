#include "../arch.hh"
#if τlinux

#include <sqlite3.h>

#include "pkv.hh"
#include "../share.hh"
#include "../begin.hh"

namespace τ
{


static M<St, Sp<sqlite3>> dbs;


static Sp<sqlite3> db_open(St filename)
{
  if (dbs.contains(filename)) return dbs.at(filename);

  sqlite3 *db;
  A(sqlite3_open(filename.c_str(), &db) == SQLITE_OK,
    "sqlite3_open(" << filename << ") error: " << sqlite3_errmsg(db));
  return dbs[filename] = Sp<sqlite3>(db, sqlite3_close);
}


static Sp<sqlite3_stmt> prepare(Sp<sqlite3> db, St sql)
{
  sqlite3_stmt *r;
  A(sqlite3_prepare_v2(db.get(), sql.c_str(), -1, &r, 0) == SQLITE_OK,
    "sqlite prepare(" << sql << ") error: " << sqlite3_errmsg(db.get()));
  return Sp<sqlite3_stmt>(r, sqlite3_finalize);
}


Γ Γpkv_get(St filename, St table)
{
  return Γffn(
    "pkv_get(" + filename + ", " + table + ")",
    [filename, table](Sp<ψ>, ξi i, ξo o)
      {
        let db = db_open(filename);
        ch *er = nullptr;
        A(sqlite3_exec(
            db.get(),
            ("create table if not exists " + table +
             "(k primary key not null, v blob)").c_str(),
            0, 0, &er) == SQLITE_OK,
          "Γpkv_get create table " << table << " failed: " << er);

        let ps = prepare(db, "select v from " + table + " where k=?");
        for (let k : i)
        {
          switch (k.t())
          {
          case η0t::bytes:   sqlite3_bind_blob(ps.get(), 1, k.data(),           k.size(), SQLITE_STATIC); break;
          case η0t::utf8:    sqlite3_bind_text(ps.get(), 1, Rc<chc*>(k.data()), k.size(), SQLITE_STATIC); break;
          case η0t::uint_be: sqlite3_bind_int (ps.get(), 1, k.pu()); break;
          case η0t::int_be:  sqlite3_bind_int (ps.get(), 1, k.pi()); break;
          default: continue;
          }

          for (int r; (r = sqlite3_step(ps.get())) != SQLITE_DONE;)
            if (r == SQLITE_ROW)
            {
              let d = Bvc{
                Rc<u8c*>(sqlite3_column_blob (ps.get(), 0)),
                Sc<uN>  (sqlite3_column_bytes(ps.get(), 0))};
              A(η0bc(d.data(), d.size()),
                "!η₀bc from sqlite, size = " << d.size());
              if (!(o << η1t(k, η0i(d.data())))) break;
            }

          sqlite3_reset(ps.get());
        }});
}


Γ Γpkv_set(St filename, St table)
{
  return Γffn(
    "pkv_set(" + filename + ", " + table + ")",
    [filename, table](Sp<ψ>, ξi i, ξo o)
      {
        let db = db_open(filename);
        ch *er = nullptr;
        A(sqlite3_exec(
            db.get(),
            ("create table if not exists " + table +
             "(k primary key not null, v blob)").c_str(),
            0, 0, &er) == SQLITE_OK,
          "Γpkv_get create table " << table << " failed: " << er);

        let ps = prepare(db, "insert or replace into " + table + "(k, v) values (?, ?);");
        for (let kv : i)
        {
          if (!kv.tT()) continue;
          let k = kv[0];
          let v = kv[1];

          switch (k.t())
          {
          case η0t::bytes:   sqlite3_bind_blob(ps.get(), 1, k.data(),           k.size(), SQLITE_STATIC); break;
          case η0t::utf8:    sqlite3_bind_text(ps.get(), 1, Rc<chc*>(k.data()), k.size(), SQLITE_STATIC); break;
          case η0t::uint_be: sqlite3_bind_int (ps.get(), 1, k.pu()); break;
          case η0t::int_be:  sqlite3_bind_int (ps.get(), 1, k.pi()); break;
          default:
            A(0, "pkv_set(" << table << "): invalid key type: " << k.t());
            return;
          }

          sqlite3_bind_blob(ps.get(), 2, v.odata(), v.osize(), SQLITE_STATIC);

          A(sqlite3_step(ps.get()) == SQLITE_DONE,
            "pkv_set(" << table << ") error: " << sqlite3_errmsg(db.get()));
          if (!(o << k)) return;
          sqlite3_reset(ps.get());
        }});
}


}

#include "../end.hh"

#endif
