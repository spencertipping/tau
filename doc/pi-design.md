# π design
As mentioned in [π](pi.md), we need a zero-copy frame of reference so we can transform individual UTF9 values without incurring piecewise overhead by copying stuff around. Imperative π should be in the same performance ballpark as Python/Perl/Ruby.

π exists to provide convenient and reasonably fast UTF9-transformation operators, which means it should be imperative and garbage-collected. The [π compiler](pi-compiler.md) should do some work to reduce complex forms down to simple ones -- i.e. most of the optimization effort belongs in the compiler rather than in the [bytecode interpreter](pi-interpreter.md).

[γ](gamma.md) operates in an implicitly vectorized context because values arrive via [φ](phi.md). The compiler should choose vectorizable bytecodes when possible to minimize interpreter overhead.


## Reddit analysis example
I recently ran a full-scale analysis to observe user and influence drift on the platform. Let's walk through all the stages.


### Step 1: ETL
```sh
$ ni /data/reddit-{comments,submissions}{,-queue} rp'r -s, a' OfB \
     fx64[%f : i%f \<D:id,:author rp'b ne "[deleted]"' zz\>] \
     \<\# ^{row/sort-buffer=65536m} gzz\>id-user-lookup.sorted

$ ni /data/reddit-submissions{,-queue} rp'r -s, a' OfB \
     fx64[%f : i%f \< D:author,:created_utc,:subreddit,:score \
               rp'a ne "[deleted]"' zz\>] \
     \<\# ^{row/sort-buffer=262144m} gABnzz\>sub-posts-sorted
```


#### `ni` performance
```sh
$ ni /data/reddit-{comments,submissions}{,-queue} \
     \<rE6\>/tmp/1m-jsons

$ time ni /tmp/1m-jsons D:id,:author rp'b ne "[deleted]"' zn
real    0m10.790s
user    0m13.664s
sys     0m5.220s

$ ni /tmp/1m-jsons D:id,:author \>/tmp/1m-extracted
$ time ni /tmp/1m-extracted rp'b ne "[deleted]"' zn
real    0m4.994s
user    0m3.666s
sys     0m0.792s
```

13.6s/1M items = 1360ns/item = 73529items/sec. The second command is a bit faster: 366ns/item = 273k items/sec. The second loop is more representative of a typical `ni` setup: the data is already de-bulked and we want to do a dense computation on a bunch of rows.


#### π bytecode analysis
The structure here is the same: for each record, select some JSON fields and then reject some values of those fields; then stream all of that into a sort. A naive π translation would involve bytecode loops that looked like this:

```
for x ← α
  a = x{"id"}
  b = x{"author"}
  if b != "[deleted]"
    β « o9t(a, b)
```

This is essentially a 1-to-1 process, so we can vectorize like this:

```
while α
  v « α
  a = v{"id"}
  b = v{"author"}
  β « o9t[b ≠ "[deleted]"](a, b)
```

We want `v{"id"}` and `v{"author"}` to happen in the same γ so we don't double-copy entries from _α_. Let's prototype this with an infeed γ that splits on newlines; then we can see how fast we are.
