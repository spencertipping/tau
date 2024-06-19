# Ωs
A search index maps a key (search term) to multiple values (documents):

```cpp
for (let &term : document)
  index.add(term, document.id);

for (let &id : index.get(term))
{
  let doc = documents[id];
  // ...
}
```

We can use [`Ωh` containers](Omega-h.md) for this because (1) they can handle multiple copies of the same k/v pair, and (2) we can store arbitrary data in `K` and `L`. The only big downside is that Ωh will copy the key for each value, which is not space-efficient. We can mitigate overhead by shortening the key size at the cost of more collisions; if we do, we'll need to check each returned document for term membership prior to committing to the results:

```cpp
V<L> get(ηi x)
{
  V<L> rc;
  let k = K{x};  // arbitrary hash transform here
  h_.get(k, [&](Lc &l) { rc.push_back(l); });

  // Now filter out the result candidates whose documents don't contain them
  V<L> r;  r.reserve(rc.size());
  for (let &l : rc)
    if (doc_at[l].has_term(x))
      r.push_back(l);
  return r;
}
```

Alternatively, it can be understood that `Ωs` provides a _superset_ of accurate results rather than an exact list. How exact it is depends on the key's bit width:

+ `Ωs32<L>`: 32-bit overhead per key, very inexact
+ `Ωs64<L>`: 64-bit overhead per key, almost exact
+ `Ωs128<L>`: 128-bit overhead per key (SHA256), exact


## Lost uniformity
Search term → document lists don't same uniformity as general 1:1 k/v pairings because we can have the same term listed many times:

```
term1 → doc1  | ← term1 may be overrepresented
term1 → doc2  |
...           |
term1 → docN  |
term2 → doc5
term3 → doc8
term4 → doc3
term4 → doc7
...
```

In practice, interpolation search is still acceptable and perhaps optimal here: we don't have a predictable bias to the distribution, so `E[k]` is still best predicted by the hash value.

**TODO:** we may benefit from defensively avoiding edge predictions to guarantee progress
