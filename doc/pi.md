# π scripting
π is a series of [φ](phi.md) parsers that allow [η](eta.md)-transforming functions to be defined from string inputs, ultimately making it possible to write many τ programs without compiling code.

π uses a simple mark/sweep GC that inlines values when they are written into others. This means we have zero-overhead η reads and linear-overhead η consing with optimal data locality.

+ [π interpreter](pi-int.md)
+ [π grammar](pi-phi.md)
+ [π function](pi-fn.md)
+ [σ/π stdlib](sigma-pi-stdlib.md)


## Blocking calls within π programs
Not all τ programs are best structured with η/ξ streams. Sometimes it makes more sense to issue direct function calls, blocking the caller and yielding until a value is returned. For example, reading stdout from a one-shot program is a blocking operation that should yield and allow other work to be done. Files are similar but require more subtle handling since Linux doesn't support async file IO with `epoll()`.

There are two ways to implement blocking calls:

1. Have π functions that manually yield and manage scheduling (e.g. `τe::Θ`)
2. Convert the call into a Γ and read the return value(s) from a ξ

Since (1) is easy, let's talk about (2). It's also pretty easy; we just need to create a ξ that accepts an initial η value (the function arguments) and then read from the resulting ξ, whether that's `fi` on the right or `bi` on the left. The π execution λ will hold a reference to the resulting ξ.

π/Γ interop can collect either _all_ ηs from the ξ (vertical), or just the first one (horizontal).

```bash
$ $sigma 'n1 p1 G>[px x+1 x+2]'
1 2 3
$ $sigma 'n1 p1 G<[|px x+1 x+2]'
1 2 3
```


## Examples/tests
**TODO:** convert these into a much more cohesive structure, where we go through different data types and operations and such

```bash
$ $sigma 'n10 px x>1 (x x>5) (x x+2 x x+1) x+1'
0 false (0 false) (0 2 0 1) 1
1 false (1 false) (1 3 1 2) 2
2 true (2 false) (2 4 2 3) 3
3 true (3 false) (3 5 3 4) 4
4 true (4 false) (4 6 4 5) 5
5 true (5 false) (5 7 5 6) 6
6 true (6 true) (6 8 6 7) 7
7 true (7 true) (7 9 7 8) 8
8 true (8 true) (8 10 8 9) 9
9 true (9 true) (9 11 9 10) 10
$ $sigma 'n3 p[[x]+1]'
1
2
3
$ $sigma 'n3 px [x]+1'
0 1
1 2
2 3
$ $sigma 'n3 px`x+1`x+2 x'
2 0
3 1
4 2
$ $sigma 'n3 p:foo x+1;$foo+1'
2
3
4
```

A few more tests:

```bash
$ $sigma 'n3 p:foo x+1 x+2 x<1;$foo $foo'
1 2 true 1 2 true
2 3 false 2 3 false
3 4 false 3 4 false
$ $sigma 'n1p[1 @(2 3)]'
1 2 3
$ $sigma 'n1p1@(2 3)'
1 2 3
$ $sigma "n3p['a x+1'b5]:'a"
1
2
3
$ $sigma 'n1p"a" "b" |- 1 2 3'
"a" "b" 1
"a" "b" 2
"a" "b" 3
τ
$ $sigma 'n1p"a" "b"-|1 2 3'
"a" 1 2 3
"b" 1 2 3
τ
$ $sigma 'n1p@_|- 1 2 3'
1
2
3
τ
$ $sigma 'n1p||- 1 2 3'
1
2
3
τ
$ $sigma 'n1p@- (1 2) (3) (4 5 6)'
1 2
3
4 5 6
τ
$ $sigma 'n1p@-(1 2)(3)(4 5 6); p>@'
((1 2) (3) (4 5 6))
$ $sigma 'n1p#=3 1 2 3 4 5 4 3 2 1'
2
$ $sigma 'n1p#=(3) (1 2) (3) (4 5) 4 (3 2) 1'
1
$ $sigma 'n1p@-(/>"/foo/bar.bif" />"bif")
                       (</"/foo/bar.bif" </"bif")
                       (.>"/foo/bar.bif" .>"bif")
                       (<."/foo/bar.bif" <."bif")'
"bar.bif" "bif"
"/foo" "bif"
"bif" "bif"
"/foo/bar" "bif"
τ
$ $sigma 'n1p"foo"++"bar\n"; >F1'
foobar
$ $sigma 'n1p 1 2 3 4; p D C B A'
4 3 2 1
```

Testing a failure case:

```bash
$ $sigma 'n1px##TEST "a" "b"'
TEST: 0 a b
0
$ $sigma 'n1px##iTEST 56 "78"'
iTEST: 0 56 78
0
```


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


## Switch/case
_x {c₁ y₁ c₂ y₂ ... z}_ evaluates the _y_ corresponding to _c = x_. If no _c = x_, then _z_ is returned.

_c_ values are not assumed to be constant; they are evaluated one-by-one, sequentially, until either _x_ is matched or we fall through to _z_.

```bash
$ $sigma 'n5 px{0 "a" 1 "b" 2 x+1 x+2}; p>@'
(("a") ("b") (3) (5) (6))
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
