#include <iostream>
#include <lmdb.h>
#include <random>
#include <chrono>
#include <vector>
#include <algorithm>

// Number of entries to insert into the database
const uint64_t NUM_ENTRIES = 100000000;

int main() {
    // Initialize LMDB environment
    MDB_env *env;
    mdb_env_create(&env);
    mdb_env_set_mapsize(env, 1024L * 1024 * 1024 * 1024); // 1 TB
    mdb_env_open(env, "./testdb", 0, 0664);

    // Begin a transaction and initialize the database
    MDB_txn *txn;
    MDB_dbi dbi;
    mdb_txn_begin(env, nullptr, 0, &txn);
    mdb_dbi_open(txn, nullptr, MDB_CREATE, &dbi);

    // Random number generator for 64-bit keys
    std::mt19937_64 rng;
    rng.seed(std::random_device{}());
    std::uniform_int_distribution<uint64_t> dist;

    // Insert 100 million entries
    std::cout << "Inserting entries..." << std::endl;
    for (uint64_t i = 0; i < NUM_ENTRIES; ++i) {
        uint64_t key = dist(rng);
        uint64_t value = key;

        MDB_val mdb_key = {sizeof(key), &key};
        MDB_val mdb_data = {sizeof(value), &value};
        mdb_put(txn, dbi, &mdb_key, &mdb_data, MDB_NOOVERWRITE);

        if (i % 1000000 == 0) {
            std::cout << i << " entries inserted" << std::endl;
        }
    }

    std::cout << "Committing transaction..." << std::endl;
    mdb_txn_commit(txn);

    // Measure random-read performance
    std::cout << "Measuring random-read performance..." << std::endl;
    const int NUM_READS = 1000000;
    std::vector<uint64_t> keys(NUM_READS);

    // Generate random keys for reads
    for (int i = 0; i < NUM_READS; ++i) {
        keys[i] = dist(rng);
    }

    // Shuffle the keys to ensure random order
    std::shuffle(keys.begin(), keys.end(), rng);

    // Begin read transaction
    mdb_txn_begin(env, nullptr, MDB_RDONLY, &txn);

    auto start = std::chrono::high_resolution_clock::now();

    for (auto& key : keys) {
        MDB_val mdb_key = {sizeof(key), &key};
        MDB_val mdb_data;
        int ret = mdb_get(txn, dbi, &mdb_key, &mdb_data);

        if (ret == MDB_NOTFOUND) {
            // Key not found, just continue
            continue;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Random-read performance: " << NUM_READS / elapsed.count() << " reads/sec" << std::endl;

    // Cleanup
    mdb_txn_abort(txn);
    mdb_dbi_close(env, dbi);
    mdb_env_close(env);

    return 0;
}
