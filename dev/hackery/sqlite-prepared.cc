#include <iostream>
#include <sqlite3.h>
#include <string.h>

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
  sqlite3_stmt *stmt;
  sql = "INSERT INTO BLOB_TEST (ID,DATA) " \
        "VALUES(?, ?);";
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

  if (rc != SQLITE_OK) {
    std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
    sqlite3_close(db);
    return 1;
  }

  int id = 1;
  const char *data = "hello world";
  int data_size = strlen(data);
  sqlite3_bind_int(stmt, 1, id);
  sqlite3_bind_blob(stmt, 2, data, data_size, SQLITE_STATIC);
  rc = sqlite3_step(stmt);

  if (rc != SQLITE_DONE) {
    std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 1;
  }

  sqlite3_finalize(stmt);

  // Find some data in the table
  sql = "SELECT DATA FROM BLOB_TEST WHERE ID=?;";
  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

  if (rc != SQLITE_OK) {
    std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
    sqlite3_close(db);
    return 1;
  }

  sqlite3_bind_int(stmt, 1, id);
  rc = sqlite3_step(stmt);
  if (rc == SQLITE_ROW) {
    const void *blob = sqlite3_column_blob(stmt, 0);
    int size = sqlite3_column_bytes(stmt, 0);
    std::cout << "Data: ";
    std::cout.write(reinterpret_cast<const char *>(blob), size) << std::endl;
  }

  sqlite3_finalize(stmt);

  // Close the database
  sqlite3_close(db);

  return 0;
}
