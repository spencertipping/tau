# η/C++ destructuring
`ηauto` provides a way to convert between C++ and η functions. For example:

```cpp
i64 f(i64 x) { return x + 1; }
F<i64(ηi)> g = ηauto(f);
```

There are two `ηauto` variants:

+ `ηauto(f)`: `f(x, y, z)` will become `g(ηi, ηi, ηi)`
+ `ηauto1(f)`: `f(x, y, z)` will become `g(ηi)`: the input is flattened

`ηauto1` is effectively `std::apply()` over `ηauto`.


## Static vs dynamic destructuring
Each function argument is type-mapped to an η extractor:

+ `i64 → y.i()`
+ `f64 → y.f()`
+ ...

This is fine for monomorphic functions, but it fails when we want to dynamic-dispatch between polymorphic variants. That is, we can't easily write `fn{}` overloads that accept multiple argument types. There are three ways to deal with this:

1. Accept generic `ηi` objects and `switch (y.t())`
2. Accept `Va<...>` to allow any of a set of types, then `std::visit`
3. Define multiple `[](...) {...}` alternatives, each of which is type-signed and inserted into a dispatch list -- so we have `ηfn(...)` that handles this for us

All three of these strategies can coexist: we just iterate through the source `ηi` types and check compatibility with each alternative for (3), or unpack into the given variant type for (2). This isn't super-fast, but it's probably fine for what we're doing -- especially if we precompute the input type using `ηi::ts()`.
