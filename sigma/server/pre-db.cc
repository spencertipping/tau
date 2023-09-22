#include <fstream>
#include <lmdb.h>

#include "pre-db.hh"
#include "../../tau/begin.hh"

namespace σ::pre
{


static M<St, Wp<lmdb_db>>   lmdbs;
static M<St, Wp<sqlite_db>> sqlites;


static uN filesize(Stc &f)
{
  std::ifstream in(f, std::ifstream::ate | std::ifstream::binary);
  if (!in.is_open()) return 0;
  return in.tellg();
}


Sp<lmdb_db> lmdb_open(Stc &f)
{
  let i = lmdbs.find(f);
  if (i != lmdbs.end() && !i->second.expired()) return i->second.lock();

  let r = Sp<lmdb_db>(new lmdb_db);
  int rc;

  uN m = std::max(filesize(f) * 2ull, 1ull << 24);  // 16MB minimum

  A((rc = mdb_env_create(&r->e))        == MDB_SUCCESS, "mdb_env_create() failed: "      << mdb_strerror(rc));
  A((rc = mdb_env_set_maxdbs(r->e, 64)) == MDB_SUCCESS, "mdb_env_set_maxdbs() failed: "  << mdb_strerror(rc));
  A((rc = mdb_env_set_mapsize(r->e, m)) == MDB_SUCCESS, "mdb_env_set_mapsize() failed: " << mdb_strerror(rc));
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
