# π scripting
π is a series of [φ](phi.md) parsers that allow [η](eta.md)-transforming functions to be defined from string inputs, ultimately making it possible to write many τ programs without compiling code.

π uses a simple mark/sweep GC that inlines values when they are written into others. This means we have zero-overhead η reads and linear-overhead η consing with optimal data locality.

+ [π interpreter](pi-int.md)
+ [π grammar](pi-phi.md)
+ [π function](pi-fn.md)
+ [σ/π stdlib](sigma-pi-stdlib.md)


## Proximal and distal values
`x` and `y` are variables meaning "this" and "that", respectively. Unary contexts use `x` alone, whereas binary contexts (like sort comparators or key/value loops) use both.

Unlike regular variables, `x` and `y` are read-only and written without the `$` prefix.


## C++ FFI and data layout
π is a scripting interface that allows C++ functions to apply to η values; as such, it provides an `auto` interface to do most of the C++ type conversions for you. This is complicated only by the fact that π values occupy two dimensions: vertical and horizontal, and those terms probably don't mean what you expect them to.

If I write an expression like `3 + 4`, `3` and `4` are _vertical_ in the sense that each occupies a separate interpreter stack entry; they are not combined into the same η stream. If, on the other hand, I write `3 4` in plural context, `3` and `4` are written sequentially into the same η buffer and occupy just one stack entry. `3` and `4` are _horizontal_. A function could access these values using a parameter of type `T<i64, i64>`.

Plurality arises in contexts where we want to be able to splice values; it's basically the list context in Perl. See [pi-phi.md](pi-phi.md) for a more technical explanation. Plural values are always horizontal because their arity is known only at runtime, whereas stack slot displacements must be known at compile-time. C++ function arguments are vertically bound to π interpreters, meaning that each arrives from a different stack slot. For example:

```cpp
f(i64, f64)           // v-unpack two stack slots, fetch first η-subvalue of each
f(i64, f64, St)       // v-unpack three stack slots
f(T<i64, f64>, St)    // access first two η subvalues of first stack slot
f(V<ηi>,       bool)  // h-unpack all η subvalues of first slot
```

Return values are tuples and are bound vertically, then horizontally:

```cpp
T<i64, St>    f(...);  // write two values onto the stack, vertically
T<T<i64, St>> f(...);  // write two values horizontally into one slot
i64           f(...);  // write a single value onto the stack (special case)
```


## Heap references, η passthrough, and `πi&`
All stack values are heap references, and functions may manipulate them directly. This can be useful when returning a reference to an η subvalue, which would involve copying if we used `auto`-conversion as outlined above. For example, here's a zero-copy map key lookup:

```cpp
πhr map_lookup(πi &i, πhr m, Stv k)
{
  let v = i[m][k];         // i[m] returns ηi, i[m][k] does the map fetch
  return i.i(m, v.one());  // create inner ref to the value, within m
}
```

GC may run if your function allocates `πi` heap memory or returns a value that does.
Your function must handle this in one of a few ways:

1. Accept a `πhr_<N>` argument to reserve `N` heap bytes before unpacking any further arguments (typically this comes first), then allocate no more than `N` bytes
2. Create a `πhlv{}` to update local `πhr`s when a GC happens, and accept all arguments only as by-value or `πhr` types
3. Construct returned values inside the function with (2), then return with `T<πhr, ...>`

For example, here's a function that returns a new map with an additional k/v binding on the end:

```cpp
πhr map_append(πi &i, πhr m, ηname k, πhr v)
{
  πhlv hv{i.h()};
  hv << m << v;  // track values for GC, which may happen during i.r()

  // Add a new value to the map, reserving two bytes for the key control+size
  // and 8 bytes for any container size changes (this is generous, but that's
  // fine).
  //
  // NOTE: we must re-dereference i[m] and i[v] inside the function because
  // i.r() may cause a GC that relocates m and v, which would invalidate the
  // resulting ηi objects. That is, i.r() creates a GC barrier between the
  // outside and inside of its lambda.
  return i.r(i[m].lsize() + k.size() + 2 + i[v].osize() + 8,
             [&](auto &&r)
    {
      // At this point we've reserved enough memory to safely write things without
      // any further GC happening. We guarantee this by creating a πhgl, which
      // locks GC while in scope.
      πhgl l{i.h()};
      r << i[m].all();       // copy existing map entries
      r << k << i[v].all();  // add new entry
    });
}
```
