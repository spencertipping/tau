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
