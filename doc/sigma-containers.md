# σ containers
As mentioned in [σ search](sigma-search.md), containers -- i.e. data structures -- follow some conventions to simplify interfacing with other components. The set of operators depends on the structure.

**TODO:** rewrite this page because it's confusing


```bash
$ bin/sigma-fast 'n1p@-(5 α)(5 ι)(6 ι)(τ)(5 ι); @?N M?>_'
5 true
6 false
τ
5 false
τ
$ bin/sigma-fast 'n1p@-("a" α 11 12)(2 α 3)("a" ι)(2 ι)(5 ι)
                       (τ)(2 ι); @:N M?>_'
"a" 11 12
2 3
5 ω
τ
2 ω
τ
$ bin/sigma-fast 'n1p@- (1 α)(2 α)(1 α)(3 α); @uN M?>_'
1
2
3
τ
```

Same is true for LMDB:

```bash
$ rm -f /tmp/test.db
$ bin/sigma-fast 'n1p@-("a" α 11 12)(2 α 3)("a" ι)(2 ι)(5 ι);
                     @:L"/tmp/test.db:foo" M?>_'
"a" 11 12
2 3
5 ω
τ
```


## Queues (`@<`, `@>`, and `@=`)
**NOTE:** for transparent FIFO queues, use the `Q` prefix. `@` means there is a visible interface that allows you to schedule and clear elements.

Queues can be cleared and await advancement signals to emit elements. They often reorder their elements, e.g. by priority. The API works like this:

```
x α → q     ⇒ x|q     # enqueue element
ι   → (q|x) ⇒ q  → x  # dequeue element if one exists
ι   → ()    ⇒ () → ω
τ   → q     ⇒ () → τ  # clear queue (sync for disk queues)
```

Queues are constructed using two suffixes: the first specifies the behavior and the second, when applicable, specifies the backend. For example:

```
@=D1G           # FIFO disk queue sized at 1GiB
@>N             # memory-backed max-priority queue
@<N             # memory-backed min-priority queue
@>S"foo.db:pq"  # sqlite-backed max-priority queue
```


## Sets (`@?`)
Sets remember whether an element has been inserted or not. Sets have the following API:

```
x α   → s   ⇒ x|s           # insert into set
x ω   → x|s ⇒ s             # delete from set (not always supported)
x ι   → s   ⇒ s → e?        # check for element existence
x ρ y → s   ⇒ s → τ[x...y]  # range query (not always supported)
τ     → s   ⇒ () → τ        # clear set (sync for disk sets)
```

Sets have some special cases:

+ `@b` to create a Bloom filter (see below)
+ `@u` to create a filter that emits only unique elements (resets on _τ_)

`@u` is a simplified set that works like this:

```
x α →   s ⇒ x|s → x
x α → x|s ⇒ x|s
τ   →   s ⇒ () → τ
```


## Maps (`@:`)
Maps are similar:

```
k  α v  → m       ⇒ {k:v,m}
k  ω    → {k:v,m} ⇒ m
k  ι    → {k:v,m} ⇒ {k:v,m} → v       # v = τ if element does not exist
k₁ ρ k₂ → m       ⇒ m → τ[k₁ ... k₂]  # range query (not always supported)
τ       → m       ⇒ () → τ
```


## Multimaps (`@;`)
Finally we have multimaps:

```
k  α v  → m                    ⇒ {k:v,m}
k  ω v  → {k:v,m}              ⇒ m
k  ω    → {k:v₁, k:v₂, ..., m} ⇒ m
k  ι    → {k:v₁, k:v₂, ..., m} ⇒ m → τ[v₁, v₂, ...]
k₁ ρ k₂ → m                    ⇒ m → τ[k₁ ... k₂]  # range (not always supported)
τ       → m                    ⇒ () → τ
```


## Search indexes (`@S`)
Search indexes are like multimaps, but always return results in sorted order. Values must be scalar, not plural. Keys take the role of terms and values are elements that should be returned for those terms.

Search terms can overflow if too many values are mapped to a term, or if the total value size is too large. You can set the limit at creation-time. Overflowed terms can no longer be used.

```bash
$ rm -f /tmp/test.db
$ bin/sigma-fast 'n1p@-("a" α 11)("a" α 12)(2 α 3)
                       ("a" ι)
                       ("a" α 4)
                       (2 ι)
                       ("a" ι)
                       (5 ι);
                     @SL"/tmp/test.db:foo" M?>_'
"a" 11 12
2 3
"a" 4 11 12
5 ω
τ
```


## Persistent collection listing (`@|`)
You can list everything in a disk-backed collection using `@|`:

```
@|L"/tmp/foo.db:t"  # list all table entries in LMDB
```


## Map/set suffixes
Like queues, maps and sets are specified using two suffixes:

```
@=N            # C++ native unbounded passthrough queue (not much point to this)
@<N            # C++ native min-queue
@?N            # C++ native set
@:N            # C++ native map
@;N            # C++ native multimap
@b318N         # C++ native bloom filter with k=3, #bits=10^18 (TODO)
@=S"foo.db:t"  # sqlite passthrough queue
@<S"foo.db:t"  # sqlite min-queue
@?S"foo.db:t"  # sqlite set
@:S"foo.db:t"  # sqlite table as map
@;S"foo.db:t"  # sqlite table as multimap
```
