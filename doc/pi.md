# π scripting
C++ gives us the flexibility to define a reasonable DSL for [γ](gamma.md), so we can write `a | b | c` to get a composite pipeline. That's a good start. Unfortunately, C++ also has some limitations:

1. It has non-instant compile times
2. Some γs require functions, which are a little verbose
3. We end up copy/pasting the admittedly small scaffold code between hard-coded examples, and writing `if()` dispatch logic to select them

π is a series of [φ](phi.md) parsers that allow γs and [η](eta.md)-transforming functions to be defined from string inputs, ultimately making it possible to write τ programs without compiling code.


## `φ<γ>`
**TODO**


## `φ<η → η>`
There are a number of cases where we have η values that need to be transformed in a simple way as they move between γs. For example, we might want to rearrange tuple elements or cons up a new tuple by duplicating a value. Other common operations include:

+ General
  + Comparing values for equality
  + Comparing values by order
  + Checking the type of a value
  + Checking for τ
+ Strings
  + Splitting a string
  + Regexing a string
  + Getting length / #occurrences of something
  + Parsing JSON into η
  + Encoding η into JSON
+ Tuples
  + Entupling a value, possibly more than once
  + Permuting a tuple: `(a, b, c) → (c, a, (b, c))` etc
+ Maps
  + Retrieving a value by key
  + Checking for a key in a map
  + Creating a map


## Asqi examples
Here are some entry points for Asqi that I would like to be able to compose:

```cpp
// heads for a repo
γfr(0) | γsplit_chr("\n") | git_heads() | γostream(std::cout)

// blobs for a revision
γfr(0) | γtsv() | git_blobs() | γostream(std::cout)

// blob contents
γfr(0) | γtsv() | git_blob_contents() | γostream(std::cout)
```

First, these should be a lot easier to write as they are:

+ `«NGh»'`
+ `«TGb»'`
+ `«TGbc»'`

It should also be simple to construct `git_heads() | git_blobs() | git_blob_contents()`, but there's some transformation involved because the signatures are:

+ `git_heads ∷ repo-path → τ[(repo-path, head-name, head-sha)]`
+ `git_blobs ∷ (repo-path, commit-sha) → τ[(commit-sha, blob-path, blob-sha)]`
+ `git_blob_contents ∷ (repo-path, blob-sha) → (blob-sha, blob-contents)`

We can build the full pipeline using some η transforms and the `γflex` connector:

+ `git_heads → (repo-path, head-name, head-sha)`
+ `map(η) → (repo-path, head-name, head-sha, (repo-path, head-sha))`
+ `γflex(git_blobs) → (repo-path, head-name, head-sha, head-sha, blob-path, blob-sha)`
+ `map(η) → (repo-path, head-name, head-sha, blob-path, (repo-name, blob-sha))`
+ `γflex(γτ(git_blob_contents)) → (repo-path, head-name, head-sha, blob-path, blob-sha, blob-contents)`

In π, we should be able to say something like this: `«NGhπ(abc(ac)) →Gbπ(abcd(ad)) →τGbc »`. Since our tuple-arrangements all append something to the input tuple, maybe we can have a macro like `π«(ac)` that adds elements. We can also drop the tuple terminators. Then we have:

```
«NGhπ+(ac →Gbπ+(ad →τGbc »
```

That looks pretty compact.


### γ and η grammars (by example)
+ `«` is a leader for `γfr`, which takes some optional configurations:
  + `fd`: integer defaulting to `0`
  + `N`, `T`, or other input filter
+ `Gx` is a shortcut for our git operators
+ `π` is a leader for `γffn` via π interpreters
  + `+` is a suffix configuration indicating `() → ()` append
  + `(` begins a tuple
    + `a` and `c` are tuple accessors: lowercase to terminate, uppercase to prefix (e.g. `Ac` would be the `c` element of the `A` sub-tuple)
  + `)` is not written but is implied; all opened tuples are closed at end of π input, which happens at whitespace here (toplevel `[]` allow whitespace in π expressions? not sure)
+ `→` is the γ flex adapter
  + `Gb` = `git_blobs`
  + `π+(ad` as above
