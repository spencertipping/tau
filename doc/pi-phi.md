# φ[π]
π's grammar is not monolithic; instead, it's a computed structure built from more fundamental parsing elements, each of which is provided by a native η function. Only the very basic syntactic elements are held constant: whitespace, comments, and `,`/`\`` separators. Everything else is configured with prefix-dispatch tables.


## Structure
```
π(a0, ea, es, os, ep, op) = (p '`')* p   # toplevel
  where p = op(a, s, p) | s
        s = os(a, s, p) | a
        a = oa(a, s, p) | a0
```

`op`, `os`, and `oa` are alternations whose elements are collected and compiled beforehand using `φauto`, with three `πf` wrapper types:

+ `πp`: a `πf` that may incorporate operators and produce multiple values
+ `πs`: a `πf` that may incorporate operators and produce a single value
+ `πa`: a `πf` that must parse just a single atom and produce a single value

`πf` is a program that can be executed by a [π interpreter](pi-int.md) and pushes its result onto the interpreter's data stack.


## `πf`/η auto
`πf` can be `ηauto`-transformed by applying a function to its data stack entries, and/or to its top value.

**NOTE:** we need two separate `ηauto` drivers to do this. One applies to η _rows_ (as `ηauto` currently does), the other applies to `πi` stack entries.
