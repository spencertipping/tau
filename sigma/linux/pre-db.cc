#include "pre-db.hh"
#include "../../tau/begin.hh"

namespace σ::pre
{


static M<St, Wp<lmdb_db>>   lmdbs;
static M<St, Wp<sqlite_db>> sqlites;


// NOTE: valgrind memory limit = 128GB, so we set mapsize to 1GB to prevent debugging errors
// https://stackoverflow.com/questions/59815026/limits-of-valgrind-on-freebsd
sletc mapsize = τdebug ? 1ull << 33 : 1ull << 40;


Sp<lmdb_db> lmdb_open(Stc &f)
{
  let i = lmdbs.find(f);
  if (i != lmdbs.end() && !i->second.expired()) return i->second.lock();

  if constexpr (τdebug)
    std::cerr << "lmdb_open(" << f << "): using 1GB instead of 1TB limit" << std::endl;

  let r = Sp<lmdb_db>(new lmdb_db);
  int rc;
  A((rc = mdb_env_create(&r->e))              == MDB_SUCCESS, "mdb_env_create() failed: " << mdb_strerror(rc));
  A((rc = mdb_env_set_mapsize(r->e, mapsize)) == MDB_SUCCESS, "mdb_env_set_mapsize() failed: " << mdb_strerror(rc));
  A((rc = mdb_env_set_maxdbs(r->e, 64))       == MDB_SUCCESS, "mdb_env_set_maxdbs() failed: " << mdb_strerror(rc));
  A((rc = mdb_env_open(r->e, f.c_str(), MDB_NOSUBDIR | MDB_NOSYNC | MDB_NOMETASYNC | MDB_NOTLS, 0664)) == MDB_SUCCESS,
    "mdb_env_open(" << f << ") failed: " << mdb_strerror(rc));
  lmdbs[f] = r;
  return r;
}

Sp<sqlite_db> sqlite_open(Stc &db)
{
  let i = sqlites.find(db);
  if (i != sqlites.end() && !i->second.expired()) return i->second.lock();

  let r = Sp<sqlite_db>(new sqlite_db);
  A(sqlite3_open(db.c_str(), &r->db) == SQLITE_OK, "sqlite3_open(" << db << ") failed");
  sqlites[db] = r;
  return r;
}


}

#include "../../tau/end.hh"
