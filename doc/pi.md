# π scripting
C++ gives us the flexibility to define a reasonable DSL for [γ](gamma.md), so we can write `a | b | c` to get a composite pipeline. That's a good start. Unfortunately, C++ also has some limitations:

1. It has non-instant compile times
2. Some γs require functions, which are a little verbose
3. We end up copy/pasting the admittedly small scaffold code between hard-coded examples, and writing `if()` dispatch logic to select them

π is a series of [φ](phi.md) parsers that allow γs and [η](eta.md)-transforming functions to be defined from string inputs, ultimately making it possible to write τ programs without compiling code.


## Evaluation model
π is a hybrid register and stack machine: we have the "current input" register for the inbound η, and the "current output" stack of `η₀o` that gets folded up at the end, or at infix write operations (`∷`).

Because π can create γs inline, there is no `φ<γ>` parser. Instead, the parser is `φ<π → γ>` with an empty π for toplevel γ construction.

Operators are left-associative unary postfix expressions with arguments on the right. So `a+b` does what you would expect, but `a+b*c` evaluates the `+` before the `*`. Precedence can be modified with `[]`.


### Parser structure
A π program is ultimately a function against a mutable interpreter. We probably want the toplevel to return a continue/stop flag when used in a loop context, so `φ<F<bool(πi&)>>`. In general, returning `true` means there's an exceptional condition and execution shouldn't continue.

`πi` keeps a register for current input; this is updated when evaluating operators. For example, `+3` is a postfix polymorphic function that applies to the current input -- so an expression like `1+3` is a series of parsers that can be applied sequentially:

+ push `1`
+ transform with `+3` (as a function)
  + `3` is parsed+pushed
  + `+` is applied to top two items

If the expression occurs within a `(` tuple context, then we also insert the "push top onto next" instruction afterwards. This instruction is implicit and exists to conserve stack arity.


### Polymorphic values
Each π value is either a η or a γ.


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
    + `a` and `c` are tuple accessors: lowercase to terminate, uppercase as a continuation (so `ac` means two distinct elements, `aC` means the `c` element of the `a` sub-tuple)
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


### Variables
I don't think variables will be common; ideally we can refer to everything by value and not store quantities. But we can allocate `:` to assign vars.

**TODO:** define a letter space for variables: Cyrillic maybe?


### String manipulation
Asqi needs to be able to detect filetypes, which in the simplest case involves looking at the extension of a filename. Bash-style `a#*.` would suffice to reduce a filename `a` to its extension (`*` defaults to greedy, we don't need `#` vs `##`).

...or maybe `↓` to drop a regex/glob. So `a↓*.` would do it.


### Conditionals
There are two common types of conditionals we might want to apply:

1. One-way check/fail, e.g. "skip blobs larger than 1MB"
2. Multi-way table delegation, e.g. "switch/case on file extension" or "take the _n_th branch"

These can all be notated with `?` followed by a series of alternatives:

```
a?1 2             ← booleans: true and false in that order
a%3?1 2 3         ← integers: 0, 1, 2, etc with catch-all at end
a?foo1 bar2 3     ← strings: choose associated branch with catch-all at end
```

Note that more than two items eliminates the boolean interpretation.

Also note that strings can be written as `'asdf` or `"asdf"` literals to support numbers and special characters.
