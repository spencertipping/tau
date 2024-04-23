#include <iostream>
#include <lmdb.h>

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <old_db> <new_db> <table>" << std::endl;
        return 1;
    }

    MDB_env* old_env;
    MDB_env* new_env;
    MDB_dbi old_db;
    MDB_dbi new_db;
    MDB_txn* old_txn;
    MDB_txn* new_txn;
    MDB_val key, data;
    MDB_cursor* cursor;

    int rc;

    // Open the old database
    rc = mdb_env_create(&old_env);
    if (rc) {
        std::cerr << "mdb_env_create old failed: " << mdb_strerror(rc) << std::endl;
        return 1;
    }
    rc = mdb_env_set_maxdbs(old_env, 64);
    if (rc) {
      std::cerr << "mdb_env_set_maxdbs old failed: " << mdb_strerror(rc) << std::endl;
      mdb_env_close(old_env);
      return 1;
    }
    rc = mdb_env_open(old_env, argv[1], MDB_RDONLY | MDB_NOSUBDIR, 0664);
    if (rc) {
      std::cerr << "mdb_env_open old failed: " << mdb_strerror(rc) << std::endl;
      mdb_env_close(old_env);
      return 1;
    }
    rc = mdb_txn_begin(old_env, NULL, MDB_RDONLY, &old_txn);
    if (rc) {
      std::cerr << "mdb_txn_begin old failed: " << mdb_strerror(rc) << std::endl;
      mdb_env_close(old_env);
      return 1;
    }
    rc = mdb_dbi_open(old_txn, argv[3], 0, &old_db);
    if (rc) {
      std::cerr << "mdb_dbi_open old failed: " << mdb_strerror(rc) << std::endl;
      mdb_txn_abort(old_txn);
      mdb_env_close(old_env);
      return 1;
    }

    // Open the new database
    rc = mdb_env_create(&new_env);
    if (rc) {
      std::cerr << "mdb_env_create new failed: " << mdb_strerror(rc) << std::endl;
      mdb_txn_abort(old_txn);
      mdb_env_close(old_env);
      return 1;
    }
    rc = mdb_env_set_maxdbs(new_env, 64);
    if (rc) {
      std::cerr << "mdb_env_set_maxdbs new failed: " << mdb_strerror(rc) << std::endl;
      mdb_env_close(old_env);
      mdb_env_close(new_env);
      return 1;
    }
    rc = mdb_env_set_mapsize(new_env, 1099511627776);
    if (rc) {
      std::cerr << "mdb_env_set_mapsize new failed: " << mdb_strerror(rc) << std::endl;
      mdb_env_close(old_env);
      mdb_env_close(new_env);
      return 1;
    }
    rc = mdb_env_open(new_env, argv[2], MDB_CREATE | MDB_NOSUBDIR, 0664);
    if (rc) {
        std::cerr << "mdb_env_open new failed: " << mdb_strerror(rc) << std::endl;
        mdb_env_close(old_env);
        mdb_env_close(new_env);
        return 1;
    }
    rc = mdb_txn_begin(new_env, NULL, 0, &new_txn);
    if (rc) {
      std::cerr << "mdb_txn_begin new failed: " << mdb_strerror(rc) << std::endl;
      mdb_env_close(old_env);
      mdb_env_close(new_env);
      return 1;
    }
    rc = mdb_dbi_open(new_txn, argv[3], MDB_CREATE, &new_db);
    if (rc) {
        std::cerr << "mdb_dbi_open new failed: " << mdb_strerror(rc) << std::endl;
        mdb_txn_abort(new_txn);
        mdb_env_close(old_env);
        mdb_env_close(new_env);
        return 1;
    }

    // Copy the data
    rc = mdb_cursor_open(old_txn, old_db, &cursor);
    if (rc) {
        std::cerr << "mdb_cursor_open failed: " << mdb_strerror(rc) << std::endl;
        mdb_txn_abort(new_txn);
        mdb_env_close(old_env);
        mdb_env_close(new_env);
        return 1;
    }
    while ((rc = mdb_cursor_get(cursor, &key, &data, MDB_NEXT)) == 0) {
        rc = mdb_put(new_txn, new_db, &key, &data, 0);
        if (rc) {
            std::cerr << "mdb_put failed: " << mdb_strerror(rc) << std::endl;
            mdb_cursor_close(cursor);
            mdb_txn_abort(new_txn);
            mdb_env_close(old_env);
            mdb_env_close(new_env);
            return 1;
        }
    }
    mdb_cursor_close(cursor);

    // Commit the transaction
    rc = mdb_txn_commit(new_txn);
    if (rc) {
        std::cerr << "mdb_txn_commit failed: " << mdb_strerror(rc) << std::endl;
        mdb_env_close(old_env);
        mdb_env_close(new_env);
        return 1;
    }

    // Close the databases
    mdb_dbi_close(old_env, old_db);
    mdb_env_close(old_env);
    mdb_dbi_close(new_env, new_db);
    mdb_env_close(new_env);

    return 0;
}
