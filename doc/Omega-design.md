# Ω design
Requirements:

+ Batched write transactions
+ Online repacking: no manual maintenance
+ Robust to corruption: at most, lose most recent _n_ transactions
+ Conditionally zero-copy reads
+ Very fast reads on NVMe
+ Moderately fast writes
+ Consistent write pausing, if any
+ Immediate consistency within one process
+ Eventual consistency across processes
+ One writer, many readers
+ Multi-writer collision avoidance (only transactions during multi-writer window can be lost)


## `h→m` split strategy
`k→v` can be reduced to `h→m`, where `h` is hash and `m` is metadata, if we store `k` and `v` indirectly at the location specified by `m`. That means we now have a simpler problem: store a mapping between two small, fixed-size quantities -- I assume 16 bytes per `hm` pair in this analysis.

We have two files for a packed table: `hm` and `kv`. `kv` is a simple linear allocation; the interesting one is `hm`, which is all about IO locality while we search for `h`. Complicating matters is that each write transaction has the potential to insert new data that we'd like to be available to anyone looking at the files; so we should make sure the database doesn't become too fragmented as values are written. Further, we can't overwrite anything a reader might be using.

+ [`k→v` value log](Omega-kv.md)
+ [`h→m` strategy](Omega-hm.md)
+ [`h→m` sorted tables](Omega-sorted.md)


## Practical notes
1. Key size is at most 25% of the database, and usually less than 5%
2. Most machines have RAM ≥ 100mNVMe space
3. With shared mappings, we can have memory = 50mNVMe
4. The average key/value mapping is 1-4kB
5. The average NVMe size is 100GB-4TB
6. Let's assume we can open up to 64 files at a time
7. Most values are just inserted and then left alone
8. Deletions tend to happen in large batches

Putting some numbers together:

```
total disk space     ≤ 4TB
mean kv mapping      ≥ 1kB
total mappings       ≤ 4B
per-mapping overhead ≤ 32 bytes
```


## Key hashing
Keys have no inherent data distribution, but `h(k)` will be uniformly-distributed; interpolation search will provide _O(lg lg n)_ lookups. Data files are always packed by `h(k)` ordering, where `h()` is suitably fast and robust. For now, `h = xxh3_128`; here are Hetzner timings:

| Operation                   | Nanoseconds |
|-----------------------------|-------------|
| `picosha2(char[64])`        | 760         |
| `xxh64(char[64])`           | 26.4        |
| `xxh3_128(char[64])`        | 26.5        |
| `unique_lock<shared_mutex>` | 7.69        |
| `shared_lock<shared_mutex>` | 5.85        |

`xxh3_128` may have minor uniformity errors, but probably not enough to offset its performance advantage over SHA256.


## Hetzner NVMe timings
Random, aligned reads across the whole disk:

| Driver            | Read size | Range | μs/iop | MiB/s  |
|-------------------|-----------|-------|--------|--------|
| `pread` direct    | 512       | 320GB | 129    | 3.79   |
| `pread` direct    | 1024      | 320GB | 129    | 7.57   |
| `pread` direct    | 4096      | 320GB | 129    | 30.28  |
| `pread` direct    | 8192      | 320GB | 145    | 53.88  |
| `pread` direct    | 16384     | 320GB | 176    | 88.78  |
| `pread` direct    | 32768     | 320GB | 213    | 146.71 |
| `pread` direct    | 65536     | 320GB | 304    | 205.59 |
| `mmap` + `memcpy` | 512       | 320GB | 140    | 3.49   |
| `mmap` + `memcpy` | 1024      | 320GB | 132    | 7.40   |
| `mmap` + `memcpy` | 4096      | 320GB | 129    | 30.28  |
| `mmap` + `memcpy` | 8192      | 320GB | 289    | 27.03  |
| `mmap` + `memcpy` | 16384     | 320GB | 530    | 29.48  |
| `mmap` + `memcpy` | 32768     | 320GB | 945    | 33.07  |
| `mmap` + `memcpy` | 65536     | 320GB | 2320   | 26.94  |

It seems like **pread() has a substantial advantage** because it can leverage the device IO queue -- whereas `memcpy()` from `mmap` repeatedly blocks to fetch more pieces. If we're reading more than one page at a time, we should prefer `pread` to `mmap`. Of course, we don't have to choose just one access mode, but `pread`'s performance is still interesting and worth knowing about.

Sequential direct IO:

| Driver   | Operation | Block size | Range  | μs/iop | MiB/s |
|----------|-----------|------------|--------|--------|-------|
| `psync`  | Overwrite | 256kiB     | 128GiB | 189    | 1323  |
| `psync`  | Read      | 256kiB     | 128GiB | 702    | 356   |
| `sync`   | Overwrite | 256kiB     | 128GiB | 181    | 1381  |
| `sync`   | Read      | 256kiB     | 128GiB | 697    | 359   |
| `libaio` | Overwrite | 256kiB     | 128GiB | 186    | 1344  |
| `libaio` | Read      | 256kiB     | 128GiB | 696    | 359   |
| `psync`  | Overwrite | 4MiB       | 128GiB | 1462   | 2736  |
| `psync`  | Read      | 4MiB       | 128GiB | 1637   | 2443  |
| `psync`  | Overwrite | 64MiB      | 128GiB | 18182  | 3520  |
| `psync`  | Read      | 64MiB      | 128GiB | 14286  | 4480  |
| `mmap`   | Overwrite | 64MiB      | 128GiB | 139665 | 458   |
| `mmap`   | Read      | 64MiB      | 128GiB | 104493 | 612   |
