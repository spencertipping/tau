# π scripting
C++ gives us the flexibility to define a reasonable DSL for [Γ](gamma.md), so we can write `a | b | c` to get a composite pipeline. That's a good start. Unfortunately, C++ also has some limitations:

1. It has non-instant compile times
2. Some Γs require functions, which are a little verbose
3. We end up copy/pasting the admittedly small scaffold code between hard-coded examples, and writing `if()` dispatch logic to select them

π is a series of [φ](phi.md) parsers that allow Γs and [η](eta.md)-transforming functions to be defined from string inputs, ultimately making it possible to write τ programs without compiling code.

See [π bindings](pi-bindings.md) for a list of functions available to user code.


## Evaluation model
π is a hybrid register and stack machine: we have the "current input" register for the inbound η, and the "current output" stack of `η₀o` that gets folded up at the end, or at infix write operations (`∷`).

Because π can create Γs inline, there is no `φ<Γ>` parser. Instead, the parser is `φ<π → Γ>` with an empty π for toplevel Γ construction.

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
Each π value is either a η or a Γ. Pretty straightforward.


### Program structure
A program is a series of `F<bool(πi&)>` functions, sort of like bytecodes but without the coding indirection since we can have closure state. Parsers produce programs that are concatenated together, hence the stack-oriented interpreter design.


## Asqi examples
Here are some entry points for Asqi that I would like to be able to compose:

```cpp
// heads for a repo
Γfr(0) | Γsplit_chr("\n") | git_heads() | Γostream(std::cout)

// blobs for a revision
Γfr(0) | Γtsv() | git_blobs() | Γostream(std::cout)

// blob contents
Γfr(0) | Γtsv() | git_blob_contents() | Γostream(std::cout)
```

First, these should be a lot easier to write as they are:

+ `«NGh»'`
+ `«TGb»'`
+ `«TGbc»'`

It should also be simple to construct `git_heads() | git_blobs() | git_blob_contents()`, but there's some transformation involved because the signatures are:

+ `git_heads ∷ repo-path → τ[(repo-path, head-name, head-sha)]`
+ `git_blobs ∷ (repo-path, commit-sha) → τ[(commit-sha, blob-path, blob-sha)]`
+ `git_blob_contents ∷ (repo-path, blob-sha) → (blob-sha, blob-contents)`

We can build the full pipeline using some η transforms and the `Γflex` connector:

+ `git_heads → (repo-path, head-name, head-sha)`
+ `map(η) → (repo-path, head-name, head-sha, (repo-path, head-sha))`
+ `Γflex(git_blobs) → (repo-path, head-name, head-sha, head-sha, blob-path, blob-sha)`
+ `map(η) → (repo-path, head-name, head-sha, blob-path, (repo-name, blob-sha))`
+ `Γflex(Γτ(git_blob_contents)) → (repo-path, head-name, head-sha, blob-path, blob-sha, blob-contents)`

In π, we should be able to say something like this: `«NGhπ(abc(ac)) ▶Gbπ(abcd(ad)) ▶τGbc »`. Since our tuple-arrangements all append something to the input tuple, maybe we can have a macro like `π«(ac)` that adds elements. We can also drop the tuple terminators. Then we have:

```
«NGhπ«(ac ▶Gbπ«(ad ▶τGbc »'
```

That looks pretty compact.

```bash
$ bin/phi "ι10↓8π:('a yy▶ϊ▶π;y+1»'"
("a",8,1)
("a",8,2)
("a",8,3)
("a",8,4)
("a",8,5)
("a",8,6)
("a",8,7)
("a",8,8)
("a",9,1)
("a",9,2)
("a",9,3)
("a",9,4)
("a",9,5)
("a",9,6)
("a",9,7)
("a",9,8)
("a",9,9)
$ bin/phi "ι10π:(yy'x)j⍕⍎"
"[0,0,"x"]"
"[1,1,"x"]"
"[2,2,"x"]"
"[3,3,"x"]"
"[4,4,"x"]"
"[5,5,"x"]"
"[6,6,"x"]"
"[7,7,"x"]"
"[8,8,"x"]"
"[9,9,"x"]"
```


### Γ and η grammars (by example)
+ `«` is a leader for `Γfr`, which takes some optional configurations:
  + `fd`: integer defaulting to `0`
+ `N` is an input filter, like `T`
+ `Gx` is a shortcut for our git operators
+ `π` is a leader for `Γffn` via π interpreters
  + `«` is a suffix configuration indicating `() → ()` append
  + `(` begins a tuple
    + `a` and `c` are tuple accessors: lowercase to terminate, uppercase as a continuation (so `ac` means two distinct elements, `aC` means the `c` element of the `a` sub-tuple)
  + `)` is not written but is implied by end-of-code
+ `▶` is the Γ flex adapter
  + `Gb` = `git_blobs`
  + `π«(ad` as above
+ `»'` is debug-out

Whitespace breaks π expressions unless `[]` are used, just like `ni` grammar. In this case we can pack them all because both `▶` and `»` are unrecognized as π commands:

```
«NGhπ«(ac▶Gbπ«(ad▶τGbc»'
```


### Variables
I don't think variables will be common; ideally we can refer to everything by value and not store quantities. But we can allocate `←` to assign vars.

I'm planning to use Chinese characters for variables.


### String manipulation
Asqi needs to be able to detect filetypes, which in the simplest case involves looking at the extension of a filename. Bash-style `a#*.` would suffice to reduce a filename `a` to its extension (`*` defaults to greedy, we don't need `#` vs `##`).

...or maybe `↓` to drop a regex/glob. So `a↓*.` would do it.
