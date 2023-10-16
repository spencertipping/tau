# π scripting
π is a series of [φ](phi.md) parsers that allow [η](eta.md)-transforming functions to be defined from string inputs, ultimately making it possible to write many τ programs without compiling code.

π uses a simple mark/sweep GC that inlines values when they are written into others. This means we have zero-overhead η reads and linear-overhead η consing with optimal data locality.

+ [π interpreter](pi-int.md)
+ [π grammar](pi-phi.md)
+ [π function](pi-fn.md)
+ [π FFI](pi-ffi.md)
+ [σ/π stdlib](sigma-pi-stdlib.md)


## Types, operators, and dialects
**A dialect is a sub-grammar optimized for a specific data type.** For example, a `M` prefix might enable a "matrix manipulation" dialect: `Mx+1` might mean "`+` is broadcast".


## Examples/tests
**TODO:** completely rework operators so they are properly organized into two-char things with mnemonic sigils

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
$ $sigma 'n1p1 2 3||-'
1
2
3
τ
$ $sigma 'n1p(1 2) (3) (4 5 6) @-'
1 2
3
4 5 6
τ
$ $sigma 'n1p(1 2)(3)(4 5 6) @- p>@'
((1 2) (3) (4 5 6))
$ $sigma 'n1p"foo"++"bar\n"; >F1'
foobar
$ $sigma 'n1p 1 2 3 4; p D C B A'
4 3 2 1
```


## Switch/case
_x {c₁ y₁ c₂ y₂ ... z}_ evaluates the _y_ corresponding to _c = x_. If no _c = x_, then _z_ is returned.

_c_ values are not assumed to be constant; they are evaluated one-by-one, sequentially, until either _x_ is matched or we fall through to _z_.

```bash
$ $sigma 'n5 px{0 "a" 1 "b" 2 x+1 x+2}; p>@'
(("a") ("b") (3) (5) (6))
```


## Proximal and distal values
`x` and `y` are variables meaning "this" and "that", respectively. Unary contexts use `x` alone, whereas binary contexts (like sort comparators or key/value loops) use both.

Unlike regular variables, `x` and `y` are read-only and written without the `$` prefix.


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
