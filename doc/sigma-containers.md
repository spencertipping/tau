# σ containers
As mentioned in [σ search](sigma-search.md), containers -- i.e. data structures -- follow some conventions to simplify interfacing with other components. The set of operators depends on the structure.


**TODO:** place keys ahead of signals, so signals are not interpreted literally; otherwise they will conflict with signals-as-flow-control


## Queues (`Q`)
FIFO queues are either passthrough or gated. Passthrough queues don't have a visible interface; they're just 1:1 against their inputs. Gated queues can be cleared and await advancement signals to emit elements. They often reorder their elements, e.g. by priority. The API works like this:

```
x → q     ⇒ x|q     # enqueue element
ι → (q|x) ⇒ q  → x  # dequeue element
τ → q     ⇒ () → τ  # clear queue
```

Queues are constructed using two suffixes: the first specifies the behavior and the second, when applicable, specifies the backend. For example:

```
Q=d1G           # passthrough disk queue sized at 1GiB
Q>              # memory-backed max-priority queue
Q<              # memory-backed min-priority queue
Q>S"foo.db:pq"  # sqlite-backed max-priority queue
```


## Maps and sets (`@`)
Maps store key/value associations, sets remember whether an element has been inserted or not. Sets have the following API:

```
α x   → s   ⇒ x|s           # insert into set
ω x   → x|s ⇒ s             # delete from set (not always supported)
ι x   → s   ⇒ s → e?        # check for element existence
ρ x y → s   ⇒ s → τ[x...y]  # range query (not always supported)
τ     → s   ⇒ () → τ        # clear set
```

Maps are similar:

```
α k v   → m       ⇒ {k:v,m}
ω k     → {k:v,m} ⇒ m
ι k     → {k:v,m} ⇒ m → v
ρ k₁ k₂ → m       ⇒ m → τ[k₁ ... k₂]  # range query (not always supported)
τ       → m       ⇒ () → τ
```

Finally we have multimaps:

```
α k v   → m                    ⇒ {k:v,m}
ω k v   → {k:v,m}              ⇒ m
ω k     → {k:v₁, k:v₂, ..., m} ⇒ m
ι k     → {k:v₁, k:v₂, ..., m} ⇒ m → τ[v₁, v₂, ...]
ρ k₁ k₂ → m                    ⇒ m → τ[k₁ ... k₂]  # range (not always supported)
τ       → m                    ⇒ () → τ
```

Like queues, maps and sets are specified using two suffixes:

```
@?             # C++ native set
@:             # C++ native map
@;             # C++ native multimap
@B318          # C++ native bloom filter with k=3, #bits=10^18
@?S"foo.db:t"  # sqlite set
@:S"foo.db:t"  # sqlite table as map
@;S"foo.db:t"  # sqlite table as multimap
```
