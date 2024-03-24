# σ containers
σ containers are C++ structures designed to make it easy to write case-specific Ψ wrappers. Because C++ provides native in-memory data structures, σ containers are focused on persistent k/v maps backed by LMDB. In particular, we define a zero-copy API that manages LMDB transactions automatically, reducing LMDB integration to a normal key/value store with RAII keys.


## Persistent collection listing (`@|`)
You can list everything in a disk-backed collection using `@|`:

```
@|L"/tmp/foo.db""t"  # list all table entries in LMDB
```
