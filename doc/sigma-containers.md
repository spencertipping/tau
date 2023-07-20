# σ containers
As mentioned in [σ search](sigma-search.md), containers -- i.e. data structures -- follow some conventions to simplify interfacing with other components. The set of operators depends on the structure.


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
QP              # memory-backed priority queue
QPS"foo.db:pq"  # sqlite-backed priority queue
```


## Maps and sets (`@`)
Maps store key/value associations, sets remember whether an element has been inserted or not. Sets have the following API:

```
α x → s   ⇒ x|s     # insert into set
ω x → x|s ⇒ s       # delete from set (not always supported)
ι x → s   ⇒ s → e?  # check for element existence
τ   → s   ⇒ () → τ  # clear set
```

**TODO:** sets and perhaps queues may need to deal with the multiple-user problem, which means we'll want socket connections for them.
