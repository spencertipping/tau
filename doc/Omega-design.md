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


## `h→m` mapping strategies
`k→v` can be reduced to `h→m`, where `h` is hash and `m` is metadata, if we store `k` and `v` indirectly at the location specified by `m`. That means we now have a simpler problem: store a mapping between two small, fixed-size quantities -- I assume 16 bytes per `hm` pair in this analysis.

So we have two files for a packed table: `hm` and `kv`. `kv` is a simple linear allocation; the interesting one is `hm`, which is all about IO locality while we search for `h`. Complicating matters is that each write transaction has the potential to insert new data that we'd like to be available to anyone looking at the files; so we should make sure the database doesn't become too fragmented as values are written. Further, we can't overwrite anything a reader might be using.

Returning to the size I mentioned earlier, how do we pack a hash, offset, and size into just 16 bytes, especially if the hash is strictly 8 bytes? It involves using just 64 bits for the offset + size, which we can do pretty easily. Let's set an upper data-file size to 16TiB, so we have an offset size of 44 bits. The remaining 19 bits lossily encode the size, which can be up to 4GiB per record:

```
exp:5      trim:15                         offset:44
|---| |----------------| |-----------------------------------------------|
eeeee ttt ttttttttt tttt oooo oooooooo oooooooo oooooooo oooooooo oooooooo
```

`offset` is literal; `size` is calculated similar to floating-point:

```
size = (1 << uint5(exp) + 1) * (1.0 - uint15(trim) / 65536.0)
```

This gives us exact packing with overreads limited to ~0.003% and lossless encoding for all objects smaller than 64kiB.


### One transaction per file
A very dumb strategy that will quickly overwhelm our 64-file limit: each transaction commits a new file of sorted data. If we have _k_ such files, lookup is _O(k lg lg n)_ IOPs.


### Sorted-run binning
We have only one file, and transactions are appended as sorted runs. However, we try to compact as we go; so once _k_ increases beyond some amount we repack the smallest runs into a larger block. Lookup performance is somewhat complex as it depends on memory latency, IO latency, _n_, _k_, `idiv`, and more:

```
get     = ∑k sr(n[k]) + kvio                // assume we search all sorted runs
sr(n)   = n < 256 ? smem(n) : sio(n)        // 256 = one page
smem(n) = min(binmem(n), ismem(n))
sio(n)  = ⌈log₂ log₂ n⌉ · (iop[16] + idiv)  // lg lg n IOPs, plus CPU overhead
```

`binmem` and `ismem` are both pretty simple; they just measure the CPU overhead of searching an array in memory, although the memory may not be hot in L3/L2 cache yet:

```
binmem(n) = ⌈log₂ n⌉      ·  l3miss
ismem(n)  = ⌈log₂ log₂ n⌉ · (l3miss + idiv)
```

Hetzner `ccx43` machine timings for `idiv` and memory accesses within various ranges, provided by `dev/hackery/cache-idiv-bench.cc`:

| Operation            | Nanos   | Cache level? |
|----------------------|---------|--------------|
| `idiv`               | 1.906   | N/A          |
| `xs[0]`              | 1.359   | L1           |
| `xs[i : 1024]`       | 1.359   | L1           |
| `xs[i : 2048]`       | 1.359   | L1           |
| `xs[i : 4096]`       | 1.359   | L1           |
| `xs[i : 8192]`       | 1.360   | L1           |
| `xs[i : 16384]`      | 1.360   | L1           |
| `xs[i : 32768]`      | 1.373   | L1           |
| `xs[i : 65536]`      | 2.718   | L2           |
| `xs[i : 131072]`     | 3.402   | L2           |
| `xs[i : 262144]`     | 3.759   | L2           |
| `xs[i : 524288]`     | 4.924   | L2           |
| `xs[i : 1048576]`    | 7.000   | L3           |
| `xs[i : 2097152]`    | 11.287  | L3           |
| `xs[i : 4194304]`    | 14.305  | L3           |
| `xs[i : 8388608]`    | 16.528  | L3           |
| `xs[i : 16777216]`   | 29.788  | memory       |
| `xs[i : 33554432]`   | 60.563  | memory       |
| `xs[i : 67108864]`   | 108.522 | memory       |
| `xs[i : 134217728]`  | 133.294 | memory       |
| `xs[i : 268435456]`  | 155.090 | memory       |
| `xs[i : 536870912]`  | 146.056 | memory       |
| `xs[i : 1073741824]` | 162.659 | memory       |
| `xs[i : 2147483648]` | 171.280 | memory       |
| `xs[i : 4294967296]` | 207.092 | memory       |
| `write(char[8])`     | 812.569 | syscall      |
| `write(char[64])`    | 829.685 | syscall      |

`idiv` is easily worth it until we're within a single cache line.


### Bloom-annotated sorted run binning
We still have _k_ runs to search through, so like one-file-per-transaction our lookup time is still _O(k lg lg n)_. That's fine if each sorted-bin lookup is just a single cheap IOP, but it's more problematic if we're hitting unpredictable regions as we go. If we've got 1B `hm` pairs, we have 16GB of sorted-bin data -- that's a lot of potential IOPs to pull from disk, almost all of which will be misses.

To avoid this, we can prepend a Bloom filter to each sorted run. Here's a table of overhead and time savings, assuming 16-byte `hm` pairs:

| FP rate | _k_ div | Hashes | Bits/key | Overhead | 1B-key BF size |
|---------|---------|--------|----------|----------|----------------|
| 0.5     | 2       | 1      | 1.44     | 1.125%   | 180MB          |
| 0.3     | 3.3     | 1.74   | 2.51     | 1.96%    | 245MB          |
| 0.1     | 10      | 3.32   | 4.79     | 3.74%    | 599MB          |
| 0.03    | 33      | 5.06   | 7.30     | 5.70%    | 913MB          |
| 0.01    | 100     | 6.64   | 9.59     | 7.49%    | 1199MB         |
| 0.003   | 333     | 8.38   | 12.09    | 9.45%    | 1511MB         |
| 0.001   | 1000    | 9.97   | 14.38    | 11.23%   | 1798MB         |

Now our performance analysis from earlier becomes a bit more complex:

```
get   = ∑k[bt(n[k]) + (1 + bfp) · sr(n[k])] + kvio
bt(n) = nh · (l3miss + idiv)
      + max(1, min(nh / (n · nB · 4096), nh)) · iop[1]
```


### Hashed-run binning
Similar to sorted-run binning, but we also split the runs by a hash value. This increases the size of the stage and decreases merge work, since each run file has only _1/p_ of the values (where _p_ is the number of hash partitions).

Hashing our database effectively divides _n_ by _p_ in exchange for multiplying memory usage by _p_.


## Appending/merging data
Any file that contains multiple runs will have a header describing them:

```
k | r1m r2m ... rKm | r1... r2... ... rK...
```

If the header has more space, we can easily just append a new run and insert its coordinates into the header as the next entry. This operation is cheaply atomic: the header update is the only critical section, and that's cheap because there are only _k_ runs and their metadata is small (16 bytes each, at most).

We can also rearrange/compact existing runs, but we have to lock the header during the whole process to avoid having readers get incomplete or corrupt data. Alternatively, we can merge into a new appended run to clear multiple existing ones; then it's just a normal append operation and we do a single header rewrite during the lock.

If we clear existing runs, our file will have free space. We can punch holes in the file to allow the underlying filesystem to reclaim that space; that's probably the simplest strategy for now. Doing this means our file is effectively append-only, and we just punch holes to free data as we go. We can also make it a point to clear adjacent regions that will later create space for data we would have appended; then it's easy to reuse space instead of appending. (ext4 limits files to 16TiB at a 4kiB block size.) Finally, we can repack the whole file into a new database, which we should probably do every so often anyway. This can happen in a background thread so we don't interrupt readers.


### Critical section timings
All of this sort/merge discussion from above applies only to the `hm` mapping files, not to the data itself. That means a 1B-key database has only 16GiB of stuff to work with -- a reasonable amount, but only a few seconds to fully compact if we use efficient IO. Put differently, it's _much worse_ to have a high _k_ than it is to compact eagerly in almost every situation. We should try to keep _k_ below 4 and maybe at 1 or 2 if we can. This can probably be a tunable parameter.


### Special `m` values
**TODO:** we can get more mileage out of subnormal `e` values, which currently have redundant encoding. This would make room for special flag values.
