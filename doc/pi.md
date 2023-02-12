# π scripting
C++ gives us the flexibility to define a reasonable DSL for [γ](gamma.md), so we can write `a | b | c` to get a composite pipeline. That's a good start. Unfortunately, C++ also has some limitations:

1. It has non-instant compile times
2. Some γs require functions, which are a little verbose
3. We end up copy/pasting the admittedly small scaffold code between hard-coded examples, and writing `if()` dispatch logic to select them

π is a series of [φ](phi.md) parsers that allow γs and [η](eta.md)-transforming functions to be defined from string inputs, ultimately making it possible to write τ programs without compiling code.


## `φ<γ>`
Simple enough: we just key each γ to a specific prefix and then have each provide a parser that consumes any configuration.

`φ<γ>` should be available within π contexts so we can define map functions that consume ηs and produce γs. It's fine if they are escaped, e.g. with an explicit `γ` prefix.


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

In π, we should be able to say something like this: `«NGhπ(abc(ac)) ▶Gbπ(abcd(ad)) ▶τGbc »`. Since our tuple-arrangements all append something to the input tuple, maybe we can have a macro like `π«(ac)` that adds elements. We can also drop the tuple terminators. Then we have:

```
«NGhπ«(ac ▶Gbπ«(ad ▶τGbc »'
```

That looks pretty compact.


### γ and η grammars (by example)
+ `«` is a leader for `γfr`, which takes some optional configurations:
  + `fd`: integer defaulting to `0`
  + `N`, `T`, or other input filter, defaulting to identity
+ `Gx` is a shortcut for our git operators
+ `π` is a leader for `γffn` via π interpreters
  + `«` is a suffix configuration indicating `() → ()` append
  + `(` begins a tuple
    + `a` and `c` are tuple accessors: lowercase to terminate, uppercase to prefix (e.g. `Ac` would be the `c` element of the `A` sub-tuple)
  + `)` is not written but is implied by end-of-code
+ `▶` is the γ flex adapter
  + `Gb` = `git_blobs`
  + `π«(ad` as above
+ `»'` is debug-out

Whitespace breaks π expressions unless `[]` are used, just like `ni` grammar. In this case we can pack them all because both `▶` and `»` are unrecognized as π commands:

```
«NGhπ«(ac▶Gbπ«(ad▶τGbc»'
```


### Tuple/map accessors
I'd like for tuple and map lookups to use overlapping grammars, which is possible if we dual-parse each η expression into separate functions, one for tuples and one for maps. The function is selected based on input type.

This means `(ac` parses into two different expressions:

+ `(η[0], η[2])` when η is a tuple
+ `(η["ac"])` when η is a map

In this case `π«` precludes the map interpretation because it forces the input to be a tuple. (Or does it? We can append map k/vs too.) But in general, expressions can have multiple meanings depending on the values they're applied to; and each such interpretation has a separate parse. A failed parse means that the expression can't be applied to that type.

**NOTE:** this mechanism requires each sub-grammar to be delineated on the same endpoint; all η-type variant parses must agree about where the π expression ends.

We can use upper/lower case to delineate map keys too. If we assume all map keys are canonically lowercase, then `abcDef` means `η["abc"]["def"]`.


## Evaluation model
π is a hybrid register and stack machine: we have the "current input" register for the inbound η, and the "current output" stack of `η₀o` that gets folded up at the end, or at infix write operations (`∷`).

We need to have a way to create γs that depend on η inputs, which means all of our `φ<γ>` parsers need a `φ<π → γ>` variant. This, in turn, implies two things:

1. The γ grammar is split into "η-parameterizable" and fixed components
2. γ parsers are defined indirectly, in terms of abstractions that allow for bifurcated constants

**Q:** should we just have everything be `φ<π → γ>` with a null π context as a default? That would simplify things a lot.
