# φ parser combinators
General-purpose string parsers that can produce various types of values. [π](pi.md) uses φ to parse functions; there's a separate grammar for [Γ](Gamma.md) pipelines.

φ is built on longest-prefix matching with delegated parsing, which enables parsers to be modified by extension libraries later on.


## φ structure
Each parser takes _(x, i)_ and returns one of:

+ _(y, j)_: parse _x → y_ and resume at _j_
+ _(e, t, j)_: return error _e_ with backtrace _t_, at _j_

Errors are always reported at the rightmost point; that is, alt-folding and nesting will preserve the most-successful parse so we can report the most local problem to the user.


## π integration
π is parsed with φ combinators that map pretty much 1:1 to language constructs. The basic structure is `φ<πf>`: a parser that produces an expression that can be evaluated when an interpreter object is passed in. From there, the grammar is split into a few cases:

```
π  :: m π | π₁ d π | π₁ t π π | π₁
π₁ :: '[' π ']' | a
```

This structure is nice because it contains no self-references except for the `[]` case, which is prefixed by a constant so we have no left recursion. It's also simple, although not so much that we should make end-users redefine the core grammar. We should have pluggable alternatives for `m`, `d`, `t`, and `π₀` and define the core grammar in terms of those. That way we can have some fixed syntax, e.g. `# ...` for line comments.
