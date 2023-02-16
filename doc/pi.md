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
A π program is ultimately a function against a mutable interpreter. We probably want the toplevel to return a continue/stop flag when used in a loop context, so `φ<F<bool(πi&)>>` -- but we wrap this in a program so we can execute multiple instructions. In general, returning `true` means there's an exceptional condition and execution shouldn't continue.

`πi` keeps a register for current input; this is updated when evaluating operators. For example, `+3` is a postfix polymorphic function that applies to the current input -- so an expression like `1+3` is a series of parsers that can be applied sequentially:

+ push `1`
+ transform with `+3` (as a function)
  + `3` is parsed+pushed
  + `+` is applied to top two items

If the expression occurs within a `(` tuple context, then we also insert the "push top onto next" instruction afterwards. This instruction is implicit and exists to conserve stack arity.


### Polymorphic values
Each π value is either a η or a γ. Pretty straightforward.


### Program structure
A program is a series of `F<bool(πi&)>` functions, sort of like bytecodes but without the coding indirection since we can have closure state. Parsers produce programs that are concatenated together, hence the stack-oriented interpreter design.


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

```bash
$ bin/phi "ι10↓8π:('foo yy▶ϊ▶[τπ:y+1]»'"
("foo",8,0)
("foo",8,1)
("foo",8,2)
("foo",8,3)
("foo",8,4)
("foo",8,5)
("foo",8,6)
("foo",8,7)
("foo",9,0)
("foo",9,1)
("foo",9,2)
("foo",9,3)
("foo",9,4)
("foo",9,5)
("foo",9,6)
("foo",9,7)
("foo",9,8)
```


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


### Variables
I don't think variables will be common; ideally we can refer to everything by value and not store quantities. But we can allocate `:` to assign vars.

**TODO:** define a letter space for variables: Cyrillic maybe?


### String manipulation
Asqi needs to be able to detect filetypes, which in the simplest case involves looking at the extension of a filename. Bash-style `a#*.` would suffice to reduce a filename `a` to its extension (`*` defaults to greedy, we don't need `#` vs `##`).

...or maybe `↓` to drop a regex/glob. So `a↓*.` would do it.
