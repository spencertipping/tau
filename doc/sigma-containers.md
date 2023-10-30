# σ containers
As mentioned in [σ search](sigma-search.md), containers -- i.e. data structures -- follow some conventions to simplify interfacing with other components. The set of operators depends on the structure.

**TODO:** rewrite this page because it's confusing


```bash
$ $sigma 'n1p(5 α)(5 ι)(6 ι)(τ)(5 ι)@-; @?N'
5 true
6 false
τ
5 false
τ
$ $sigma 'n1p("a" α 11 12)(2 α 3)("a" ι)(2 ι)(5 ι)
             (τ)(2 ι)@-; @:N'
"a" 11 12
2 3
5 ω
τ
2 ω
τ
$ $sigma 'n1p(1 α)(2 α)(1 α)(3 α)@-; @uN'
1
2
3
τ
```

Same is true for LMDB:

```bash
$ rm -f /tmp/test.db
$ $sigma 'n1p("a" α 11 12)(2 α 3)("a" ι)(2 ι)(5 ι)@-
          @:L"/tmp/test.db:foo"'
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
Finally we have multimaps, which store multiple values per key. We always return the values in a deterministic order, which may or may not be η-sorted. Values must be scalar, not plural. Multimaps are usually based on regular key/value maps and do not duplicate keys.

```
k  α v  → m                    ⇒ {k:v,m}
k  ω v  → {k:v,m}              ⇒ m
k  ω    → {k:v₁, k:v₂, ..., m} ⇒ m
k  ι    → {k:v₁, k:v₂, ..., m} ⇒ m → τ[v₁, v₂, ...]
k₁ ρ k₂ → m                    ⇒ m → τ[k₁ ... k₂]  # range (not always supported)
τ       → m                    ⇒ () → τ
```

Internally, multimaps are used by search indexes (`@S`). Multimap implementations prevent quadratic insertion times by staging incremental updates and zipping them together when they are sufficiently large. Multimaps provide the `κ` signal as a manual compaction operator:

```
k κ   # compact a key
κ     # compact all keys
```

Internally, we arrange this by wrapping the underlying map with control mappings, which take one of these forms:

+ `0 k → 0 xs...`: `xs...` are the values (simple case for fast small keys)
+ `0 k → N ks...`: `M₁ k₁ M₂ k₂ ...`: values are unioned from `k₁`, `k₂`, etc.
+ `1 k₁ → M xs...`: `k₁` is generated internally, and is one of the `ks...` entries; there are `M` `xs...`, which are values being stored

`xs...` are always sorted so we can zipper-union them for retrieval. When `N` is too large, we compact the key by combining the smallest sub-keys. The goal is to flatten the structure without spending too much effort.

As for insertions, we always do one of two things:

1. If the smallest key is sufficiently small or we are within half its size, then we merge with it directly
2. Otherwise, we create a new key and defer the merge

This results in at-worst halving sizes on average.


## Search indexes (`@S`)
Search indexes use multimaps under the hood. Values must be scalar, not plural. Keys take the role of terms and values are elements that should be returned for those terms.

Search terms can overflow if too many values are mapped to a term, or if the total value size is too large. You can set the limit at creation-time. Overflowed terms can no longer be used.

```bash
$ rm -f /tmp/test.db /tmp/test.db-lock
$ $sigma 'n1p("a" α 11)
             ("a" α 13)
             (2 α 3)
             ("a" ι)
             ("a" α 12)
             ("a" α 16)
             ("a" α 13)
             ("a" α 4)
             (2 ι)
             ("a" ι)
             (5 ι)@-
          @SL"/tmp/test.db:foo"'
"a" 11 13
2 3
"a" 4 11 12 13 16
5
τ
```

Persistent search collections can support multiple simultaneous readers and writers within the same process:

```bash
$ rm -f /tmp/test.db /tmp/test.db-lock
$ $sigma 'n1p("a" ι)
             ("a" α "foo")
             ("a" κ)
             ("a" ι)
             ("a" α "bar")
             ("a" κ)
             ("a" ι)@-
          K @SL"/tmp/test.db:foo"'
"a"
"a" "foo"
"a" "bar" "foo"
$ rm -f /tmp/test.db /tmp/test.db-lock
$ $sigma 'n1p("q" "a" ι)
             ("i" "a" α "foo")
             ("i" "a" α "bar")
             ("q" "a" ι)
             ("i" τ)
             ("i" ω)
             ("q" "a" ι)@-
          K p0.01Θ.`x;
          {i|@SL"/tmp/test.db:foo"
           q|@SL"/tmp/test.db:foo"}'
"q" "a"
"q" "a" "bar" "foo"
"i" τ
"i" ω
"q" "a" "bar" "foo"
"q" ω
```

Queries are issued as _n ρ i q_, where _n_ is the maximum number of results to return, _i_ is the query ID (emitted with results), and _q_ is a recursively-defined query:

+ `("+" a b c ...)`: union queries
+ `("*" a b c ...)`: intersect queries
+ `("-" a b)` subtract `b`'s results from `a`
+ `("t" x)` retrieve term `x`, just like _(x ι)_

```bash
$ rm -f /tmp/test.db /tmp/test.db-lock
$ $sigma 'n1p("a" α "foo")
             ("a" α "bar")
             ("b" α "foo")
             ("b" α "bif")
             ("c" α "foo")
             ("c" α "bar")
             ("c" α "bok")
             (10 ρ "q1" ("t" "a"))
             (10 ρ "q2" ("*" ("t" "a") ("t" "b")))
             (10 ρ "q3" ("*" ("t" "a") ("t" "c")))
             (10 ρ "q4" ("+" ("t" "a") ("t" "b")))
             (10 ρ "q5" ("+" ("t" "b") ("t" "c")))
             (10 ρ "q6" ("-" ("t" "b") ("t" "a")))
             @-
          K @SL"/tmp/test.db:foo"'
"q1" "bar" "foo"
"q2" "foo"
"q3" "bar" "foo"
"q4" "bar" "bif" "foo"
"q5" "bar" "bif" "bok" "foo"
"q6" "bif"
```

**TODO:** add term-vector overflow size and maybe auto-commit size to `@S` grammar


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
