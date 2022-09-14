# π types
π has a type system, but it's not principally about type _of values_. Instead, it's about variants of functions. Sometimes this involves specializing a function to a specific kind of value (e.g. `+` over vectors/tuples/strings), other times it can be in response to a constraint-as-test-case that reduces the space of acceptable implementations (e.g. to force a certain level of floating-point precision).

Everything about a function, from its name to its argument types, is a search term used to select an implementation and, in some cases, argument/return coercions. π programs are generally underconstrained, which means the compiler can use discretion and choose function variants to optimize: for debuggability, space complexity, or time complexity, for instance.


## Specifying programs
Let's say that _a program is fully specified when a compiler can infer any ambiguities in the way you would have wanted it to._ This places a dual obligation on the programmer and the compiler: the programmer must fully understand how the compiler will interpret constraints, and the compiler must, within its degrees of freedom, fully understand what the programmer wants. Taking the programmer's side as a given, let's circumscribe what the compiler might reasonably be able to understand:

1. The program should compile
2. The program should not fail at runtime
3. The program should use minimal time
4. The program should use minimal space
5. Where (4) and (3) conflict, use available space if it saves time
6. The compiler-specified parts of the program should be minimally surprising to the programmer

In order to do this well, π needs three features:

1. A way to rule out certain possibilities
2. A gradient function that specifies the best of available options
3. Realtime visual feedback describing the inferences it's making


### Chain splicing
Most languages have an ergonomic problem in that they don't leave room for adapters to do trivial conversions. For example, `map $_ =~ /foo/, @xs` requires `map` to be written, even though I obviously mean `@xs =~ /foo/`. And the `=~` is also unnecessary; if I put a regex near a value, it's meant to be applied: `/foo/@xs`.

π would see `/foo/@xs` (**NOTE:** not real syntax) as `(/foo/, ·, @xs)`, where `·` is a splice cue -- that is, an invitation for π to write low-entropy code.


### Code from examples
In the limit, π can write entire functions for you given nothing but examples. For example:

```
{"name":"bar", "items": [1, 2, 3]}  →  {"bar":3}
```

There are a few functions that satisfy this constraint, sorted here by descending probability:

1. `{x.name: x.items.length}`
2. `{x.name: x.items[-1]}`
3. `{x.name: 3}`
4. `{"bar": ...}`

Notice that inserting constants into the output is done as a last resort; these are considered artificially high-entropy compared to input-transforms (reason being, you've requested an algorithm and not a template). `.length` is more common than `[-1]`, so we prefer it -- and in this case it seems intuitively correct.

It's fine for π to try lots of different permutations; the most important thing is that it follows the entropy gradient closely.


## Type structure
Unlike in most languages, π types don't describe value representation in any way. Instead, values are untyped UTF9 streams and types are contracts between functions that operate on them. In this sense, all types are fully abstract -- which means we can open up the language quite a lot.

Structurally, a type is a UTF9 value made of tuples, symbols, and integers. Symbols are knowns, integers are compactly-indexed unknowns. π's goal is to assign symbols to the unknowns.


## Function metadata
A function is specified by its value-contract endpoints and by constraints on its name: `f ∷ a → b` -- or, in π terms, `(f, a, b)`. Any subset of these can be unknown, including `f`. Missing pieces are entropy-graded as discussed earlier.

Endpoints are anchor constraints: `i64`, `json`, `file`, `laptop`, `server`, `human`.


## Constraint solver structure
**TODO**
