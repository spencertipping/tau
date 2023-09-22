#include "pre-db.hh"
#include "../../tau/begin.hh"
#include <lmdb.h>

namespace σ::pre
{


static M<St, Wp<lmdb_db>>   lmdbs;
static M<St, Wp<sqlite_db>> sqlites;


Sp<lmdb_db> lmdb_open(Stc &f)
{
  let i = lmdbs.find(f);
  if (i != lmdbs.end() && !i->second.expired()) return i->second.lock();

  let r = Sp<lmdb_db>(new lmdb_db);
  int rc;
  A((rc = mdb_env_create(&r->e))        == MDB_SUCCESS, "mdb_env_create() failed: "     << mdb_strerror(rc));
  A((rc = mdb_env_set_maxdbs(r->e, 64)) == MDB_SUCCESS, "mdb_env_set_maxdbs() failed: " << mdb_strerror(rc));
  A((rc = mdb_env_open(r->e, f.c_str(), MDB_NOSUBDIR | MDB_NOSYNC | MDB_NOMETASYNC | MDB_NOTLS | MDB_NORDAHEAD, 0664)) == MDB_SUCCESS,
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
