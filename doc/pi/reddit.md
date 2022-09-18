# Reddit analysis example
I recently ran a full-scale analysis to observe user and influence drift on the platform. Let's walk through all the stages.


## Step 1a: ETL
```sh
$ ni /data/reddit-{comments,submissions}{,-queue} rp'r -s, a' OfB \
     fx64[%f : i%f \<D:id,:author rp'b ne "[deleted]"' zz\>] \
     \<\# ^{row/sort-buffer=65536m} gzz\>id-user-lookup.sorted

$ ni /data/reddit-submissions{,-queue} rp'r -s, a' OfB \
     fx64[%f : i%f \< D:author,:created_utc,:subreddit,:score \
               rp'a ne "[deleted]"' zz\>] \
     \<\# ^{row/sort-buffer=262144m} gABnzz\>sub-posts-sorted
```


### `ni` performance
```sh
$ ni /data/reddit-{comments,submissions}{,-queue} \
     \<rE6\>/tmp/1m-jsons

$ time ni /tmp/1m-jsons D:id,:author rp'b ne "[deleted]"' zn
real    0m3.293s
user    0m3.743s
sys	    0m0.923s

$ ni /tmp/1m-jsons D:id,:author \>/tmp/1m-extracted
$ time ni /tmp/1m-extracted rp'b ne "[deleted]"' zn
real    0m1.604s
user    0m0.865s
sys	    0m0.190s
```

If we just look at user time, we have a total of 3.743s for 1M items, which comes to 374ns/item and 267k items/second.


### π bytecode analysis
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

I wrote a short test function in C++ to see what the performance limit would be, assuming no interpreter overhead. Here's the code:

```cpp
#include "../dep/rapidjson-h/document.h"
// ...
fd_in(f, open("/tmp/1m-jsons", O_RDONLY))
  | split_chr(f, '\n')
  | *new ϝ(f, "json", ϝ::ξι, [](ϝ &f)
    { Document d;
      for (let x : f)
      { d.Parse(Rc<chc*>(x.data()), x.size());
        if (!(f.β() << o9t(d["id"].GetString(),
                           d["author"].GetString())))
          break; }})
  | *new ϝ(f, "filter", ϝ::ξι, [](ϝ &f)
    { for (let x : f)
        if (x[1] != "[deleted]")
          if (!(f.β() << x)) break; })
  | stream_out(f, cout);
```

That produces:

```
$ time try/pi-design > /dev/null
real    0m2.680s
user    0m2.396s
sys     0m0.284s
```

This means we're about twice as fast as `ni`, yielding 239ns/item, or 417k items/second. That's not bad at all. Just out of curiosity, how does `jq` stack up?

```sh
$ time ni /tmp/1m-jsons e[jq -c -r '[.id, .author] | @tsv'] \
          rp'b ne "[deleted]"' zn
real    0m9.205s
user    0m8.820s
sys	    0m1.252s
```

Not well. Unsurprising given that `ni` cheats a lot in its JSON parsing, and we're using a performance-tuned library. This means τ is likely to be ~4x faster than existing JSON-parsing workflows.


### π code design
`ni` is pretty compact: the first part of the data workflow above amounts to `D:id,:author rp'b ne "[deleted]"'`. `D` and `rp` are both stream operators, so in π terms they'd have type `φ a → φ b`. `D` is a little more specific, in that it's `φ JSON → φ a`.

π programs can be data-_hinted_ but they can't use runtime data to inform compile-time type inference. (It's fine for a program to quote some development data into the code and infer from there.) If we quote a few inputs, we can abbreviate key names and infer the input type; then our π code might look like this:

```
(.id,.au) %B!/^\[/
```

The function must take the form `φ (json {'id: 'int, 'author: 'utf8, ...}) → a`; parsing is driven by the left type here.

+ `(.id,.au) ∷ φ (json {'id: 'int, 'author: 'utf8, ...}) → φ ('int, 'utf8)`
  + `(f, g) ∷ m a → m (f a, g a)`
  + `.id ∷ m (json {'id*: 'int,  ...}) → m 'int`
  + `.au ∷ m (json {'au*: 'utf8, ...}) → m 'utf8`

Now the left type is `φ ('int, 'utf8)`, which drives the parse further. `φ a` provides `% ∷ φ a → (m a → m 'bool) → φ a`:

+ `(.id,.au) %B!/^\[/ ∷ φ ('int, 'utf8)`
  + `B :: m (a, b, ...) → m b`
  + `! :: m a → (m a → m 'bool) → m 'bool`
  + `/^\[/ :: m 'utf8 → m 'bool`

Note that `!` and `/^\[/` both have polymorphic return types. Also note that at parse-time, `a` has already been unified to `'utf8` -- which is why `/` is interpreted as a regex rather than a vector reduction.


## Step 1b: distributed work
Some of the ETL in step 1a is better understood on its own:

```
fx64[%f : i%f \< ... zz\>] \<\# gzz
```

This is clumsy and basic map/reduce machinery. I like the fact that it's manually specified in `ni`; it means we aren't framework-bound. The downside is that some amount of map/reduce complexity is worth having: multi-machine workflows and data locality for example.


### π and distributed streams
If `φ` is a single stream, then `φs` can be multiple streams, e.g. in a map/reduce context.

**TODO:** define what "multiple streams" means here -- they can just be plural, or they can carry information about how they were sharded
