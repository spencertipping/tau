# π bindings
## Literals
+ `'foo`: string `foo` (cannot contain whitespace or delimiters)
+ `"foo"`: string `foo` (can contain most characters)
+ integers
+ floats (**TODO:** fix)
+ signals (`α`, `κ`, `ι`, `τ`, and `ω`)
+ `# ...`: line comment


## Groups
+ `(xyz`: tuple, closing `)` is optional
+ `{foo1 bar2`: map, closing `}` is optional, keys are literal strings
+ `[...`: subexpression, closing `]` is optional
+ `λ...`: quote `...` as a function, push offset (not commonly used)


## Accessors
+ `y`: current input
+ `a`..`i`: `y[0]`..`y[8]`


## Control flow
+ `a⋄b`: evaluate `a` and `b`, return `b`


## Debug
+ `x::I`: print interpreter state, return `x`
+ `x::'foo`: print `x` to stderr, prefixed with `foo: ` tag


## Strings
+ `(1,2,3)j`: convert `(1,2,3)` to JSON string
+ `"[1,2,3]"J`: convert `"[1,2,3]"` from JSON to η


## Tuples
+ `A`..`I`: access tuple element `0`..`8`
