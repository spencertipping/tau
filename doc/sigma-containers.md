# σ containers
As mentioned in [σ search](sigma-search.md), containers -- i.e. data structures -- follow some conventions to simplify interfacing with other components. The set of operators depends on the structure.


```bash
$ bin/sigma-fast 'n1p@-(α 5)(ι 5)(ι 6)(τ)(ι 5); @? M?>_'
true
false
τ
false
τ
$ bin/sigma-fast 'n1p@-(α "a" 11 12)(α 2 3)(ι "a")(ι 2)(ι 5)
                       (τ)(ι 2); @: M?>_'
11 12
3
τ
τ
τ
τ
$ bin/sigma-fast 'n1p@- (α 1)(α 2)(α 1)(α 3); @u M?>_'
1
2
3
τ
```


## Queues (`@<`, `@>`, and `@=`)
**NOTE:** for transparent FIFO queues, use the `Q` prefix. `@` means there is a visible interface that allows you to schedule and clear elements.

Queues can be cleared and await advancement signals to emit elements. They often reorder their elements, e.g. by priority. The API works like this:

```
α x → q     ⇒ x|q     # enqueue element
ι   → (q|x) ⇒ q  → x  # dequeue element if one exists
τ   → q     ⇒ () → τ  # clear queue
```

Queues are constructed using two suffixes: the first specifies the behavior and the second, when applicable, specifies the backend. For example:

```
@=d1G           # FIFO disk queue sized at 1GiB
@>              # memory-backed max-priority queue
@<              # memory-backed min-priority queue
@>S"foo.db:pq"  # sqlite-backed max-priority queue
```


## Sets (`@?`)
Sets remember whether an element has been inserted or not. Sets have the following API:

```
α x   → s   ⇒ x|s           # insert into set
ω x   → x|s ⇒ s             # delete from set (not always supported)
ι x   → s   ⇒ s → e?        # check for element existence
ρ x y → s   ⇒ s → τ[x...y]  # range query (not always supported)
τ     → s   ⇒ () → τ        # clear set
```

Sets have some special cases:

+ `@B` to create a Bloom filter (see below)
+ `@u` to create a filter that emits only unique elements (resets on _τ_)

`@u` is a simplified set that works like this:

```
α x →   s ⇒ x|s → x
α x → x|s ⇒ x|s
τ   →   s ⇒ () → τ
```


## Maps (`@:`)
Maps are similar:

```
α k v   → m       ⇒ {k:v,m}
ω k     → {k:v,m} ⇒ m
ι k     → {k:v,m} ⇒ {k:v,m} → v       # v = τ if element does not exist
ρ k₁ k₂ → m       ⇒ m → τ[k₁ ... k₂]  # range query (not always supported)
τ       → m       ⇒ () → τ
```


## Multimaps (`@;`)
Finally we have multimaps:

```
α k v   → m                    ⇒ {k:v,m}
ω k v   → {k:v,m}              ⇒ m
ω k     → {k:v₁, k:v₂, ..., m} ⇒ m
ι k     → {k:v₁, k:v₂, ..., m} ⇒ m → τ[v₁, v₂, ...]
ρ k₁ k₂ → m                    ⇒ m → τ[k₁ ... k₂]  # range (not always supported)
τ       → m                    ⇒ () → τ
```


## Map/set suffixes
Like queues, maps and sets are specified using two suffixes:

```
@=             # C++ native unbounded passthrough queue (not much point to this)
@<             # C++ native min-queue
@?             # C++ native set
@:             # C++ native map
@;             # C++ native multimap
@B318          # C++ native bloom filter with k=3, #bits=10^18
@=S"foo.db:t"  # sqlite passthrough queue
@<S"foo.db:t"  # sqlite min-queue
@?S"foo.db:t"  # sqlite set
@:S"foo.db:t"  # sqlite table as map
@;S"foo.db:t"  # sqlite table as multimap
```
