#include <iostream>
#include <sqlite3.h>

int main() {
  sqlite3 *db;
  char *err_msg = 0;
  int rc = sqlite3_open("test.db", &db);

  if (rc != SQLITE_OK) {
    std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
    sqlite3_close(db);
    return 1;
  }

  // Create a table
  const char *sql = "CREATE TABLE IF NOT EXISTS BLOB_TEST("  \
                    "ID INT PRIMARY KEY     NOT NULL," \
                    "DATA           BLOB    NOT NULL);";

  rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

  if (rc != SQLITE_OK) {
    std::cerr << "SQL error: " << err_msg << std::endl;
    sqlite3_free(err_msg);
    sqlite3_close(db);
    return 1;
  }

  // Insert some data into the table
  sql = "INSERT OR REPLACE INTO BLOB_TEST (ID,DATA) " \
        "VALUES(1, X'68656c6c6f20776f726c64');";
  rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

  if (rc != SQLITE_OK) {
    std::cerr << "SQL error: " << err_msg << std::endl;
    sqlite3_free(err_msg);
    sqlite3_close(db);
    return 1;
  }

  // Find some data in the table
  sql = "SELECT DATA FROM BLOB_TEST WHERE ID=1;";
  rc = sqlite3_exec(db, sql, [](void *data, int argc, char **argv, char **azColName) -> int {
    std::cout << "Data: " << argv[0] << std::endl;
    return 0;
  }, 0, &err_msg);

  if (rc != SQLITE_OK) {
    std::cerr << "SQL error: " << err_msg << std::endl;
    sqlite3_free(err_msg);
    sqlite3_close(db);
    return 1;
  }

  sqlite3_close(db);
  return 0;
}
