# π interpreter
A π program is just a C++ lambda of the form `F<πv(πi&)>`, where `πi` is a structure that stores several elements:

+ `ξi`: current input
+ `ξo`: current output
+ `M<St, πv>`: bound variables

The return value is the evaluated value of the expression, after side effects against `πi`.

**TODO:** rework this; we should use a single η heap buffer with a stack of offset registers and shallow GC. Duplicating values for C++ heap encoding is slow and insane.


## `πv` runtime values
`πv` can encode each of the alternatives in [η](eta.md), plus some that can't be serialized:

+ C++ native (stored on heap)
  + `τ::Re`
  + `τ::V<πv>`
  + `τ::M<St, πv>`
  + `τ::V<i8>`
  + `τ::V<i16>`
  + `τ::V<i32>`
  + `τ::V<i64>`
  + `τ::V<f32>`
  + `τ::V<f64>`
+ η-serializable (stored immediately)
  + `τ::ηsig`
  + `τ::i64`
  + `τ::f64`
  + `τ::St`
  + `τ::ηatom`
  + `τ::η` (for sub-η)
  + `τ::P<τ::St, τ::η>` for name prefixes
  + `τ::Sn<τ::i8b>`
  + `τ::Sn<τ::i16b>`
  + `τ::Sn<τ::i32b>`
  + `τ::Sn<τ::i64b>`
  + `τ::Sn<τ::f32b>`
  + `τ::Sn<τ::f64b>`

`πv`s are indirected through `Sp<>` to allow for low-effort copying.


## Operator implementations
An expression like `3+4` is parsed into three `πf` functions:

+ `3` → `[](πi&) { return πv{3}; }`
+ `4` → `[](πi&) { return πv{4}; }`
+ `+` → `[f, g](πi&) { return πv{f() + g()}; }`

There's a bit more nuance to `+`, however, because multiple types can be added with different C++ semantics. The `+` operator behavior is encoded as a separate polymorphic function that looks like this:

```
let plus = fn {
  [](auto  &&x, auto &&y) { type_error(x, y); },
  [](int     x, int    y) { return x + y; },
  [](double  x, double y) { return x + y; },
  [](Stc    &x, auto  &y) { return (Ss{} << x << y).str(); },
  [](auto   &x, Stc   &y) { return (Ss{} << x << y).str(); },
  [](Sn<i8> &x, int    y) { return broadcast_left(+, x, y); },
  ...
};
```

We then invoke it with `std::visit` to calculate the result across all of the variant alternatives.
