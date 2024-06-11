# Ω `h→m` sorted record storage
We need to be able to append new sorted `h→m` record bundles without rewriting the whole index, and in a way that supports transactions. There are a few different high-level strategies we can use.


## One transaction per file
A very dumb strategy that will quickly overwhelm our 64-file limit: each transaction commits a new file of sorted data. If we have _k_ such files, lookup is _O(k lg lg n)_ IOPs.


## Sorted-run binning
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


## Bloom-annotated sorted run binning
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


## Hashed-run binning
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


## Critical section timings
All of this sort/merge discussion from above applies only to the `hm` mapping files, not to the data itself. That means a 1B-key database has only 16GiB of stuff to work with -- a reasonable amount, but only a few seconds to fully compact if we use efficient IO. Put differently, it's _much worse_ to have a high _k_ than it is to compact eagerly in almost every situation. We should try to keep _k_ below 4 and maybe at 1 or 2 if we can. This can probably be a tunable parameter.
