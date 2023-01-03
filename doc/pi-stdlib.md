# π standard library
The standard library ships in versions that you can enable per program, so I don't have to be especially careful about backwards compatibility.

See also [π ABI2/linux](pi-linux.md) and [π ABI2/WASM](pi-wasm.md).


## Argument ordering
Most commands order their arguments like this:

```
args... object? [f₁] [f₂]... command
```

In other words, arguments are deepest on the stack, followed by the object if there is one, followed by any config-level literals (some of which are functions). So `i8.` would be used as `n [f] i8.`, and `φ<` is `x φ φ<`.

Argument order is reversed from infix: `x y f32>` is equivalent to `(y > x)`.


## Stack manipulation
Anything complicated will use frames, but it's good to have a few functions to minimize the GC impact of having frame-scoped data.

+ `:`: duplicate top element
+ `::`: duplicate top pair of elements
+ `^`: grab second element
+ `^^`: grab third element
+ `_`: drop
+ `%`: swap


### Frames
Mostly enabled with syntax, but we should also have explicit instructions:

+ `n &:`: push a frame with _n_ locals
+ `&_`: drop frame
+ `n &@`: get local
+ `x n &=`: set local

`&@` and `&=` are slightly slower than named variable accessors because `n` is pushed onto the stack.


### Globals
+ `'x %@`: get global
+ `v 'x %=`: set global
+ `'x %_`: delete global
+ `'x %?`: does global exist


## Control flow
+ `[...] .`: run bytecode function
+ `c [t] [e] ?.`: if `c` is true, run `[t]`; else run `[e]`
+ `c t e ?`: if-then-else
+ `[b] [c] ?!`: while `[c]` returns true, do `[b]`
+ `[b] !!`: run `[b]` forever
+ `.^`: pop return stack (used to early-return or break a loop)


### Interpreter control
**TODO:** `spush`/`spop`, run-catching-exceptions, GC, timings, etc -- currently these are debugging functions


## UTF9
### General
+ `?e`: does the value exist?
+ `?t`: what is the value type? (returns symbol)
+ `?f`: is the value flagged?
+ `?s`: how big is the value? (inner size)
+ `?S`: outer size
+ `:f`: flatten value (inlines all references)
+ `:h`: hash value
+ `$b`: UTF9 → `bytes` (flatten + serialize)
+ `b$`: `bytes` → UTF9 -- deserialize and validate (invalid → `none`)
+ `b$!!`: `bytes` → UTF9 -- deserialize and don't validate (dangerous)
+ `==`: check for equality
+ `≡`: check for referential equality


### Structure literals
+ `(`: begin tuple (fork stack)
+ `)`: end tuple (pop forked stack)
+ `{`: begin map
+ `}`: end map
+ `,`: emit tuple element


### Vectors
**NOTE:** we define `uN` and `iN` as aliases for platform-native ints, so you can say `>uNs` to get a vector of native-sized unsigned ints if you want to.

In the functions below, `v` is replaced by a primitive type, e.g. `i8s`. So `i8s#` returns the number of elements in a vector of `i8`s.

+ `xs >v`: tuple/set/bytes/UTF8 to typed vector
+ `... x₃ x₂ x₁ n >>v`: make typed vector
+ `xs v#`: vector length
+ `xs i v@`: get vector element
+ `xs i n v!`: sub-vector
+ `a b v++`: concatenate vectors
+ `xs [...] v.`: vector for-each
+ `xs [...] v*`: vector elementwise map (**FIXME:** ambiguous)
+ Native reductions
  + `xs v/+`: sum of elements
  + `xs v/*`: product of elements
  + `xs v/<`: check for ascending order (returns true or false)
  + `xs v/&`: bitwise-and of elements
  + `xs v/|`: bitwise-or of elements
  + `xs v/^`: bitwise-xor of elements


### Numbers
All of these operators automatically distribute across vectors.

+ Type conversions
  + `>i8`: saturate on overflow (TODO; right now overflow is undefined)
  + `>i16`
  + `>i32`
  + `>i64`
  + `>u8`: truncate on overflow/underflow
  + `>u16`
  + `>u32`
  + `>u64`
  + `>f32`
  + `>f64`
  + `>c32`: assign real part only
  + `>c64`
+ General arithmetic
  + `i8+`: undefined on overflow
  + `i16+`
  + ...
  + `u64+`: wrap on overflow/underflow
  + `i8-`, ...
  + `i8*`, ...
  + `i8**`, ...
  + `i8/`, ...
  + `i8%`, ...
  + `i8n`, ...: negate
  + `i8min`, ...
  + `i8max`, ...
  + `i8abs`, ...
+ Relational
  + `i8==`, ...: return bool vector
  + `i8!=`, ...
  + `i8>`, ...
  + `i8<`, ...
  + `i8>=`, ...
  + `i8<=`, ...
+ Integer arithmetic
  + `i8l`, ...: nonzero → 0, 0 → 1 (`l` instead of `!`, which means "substring" in π)
  + `u8~`, ...
  + `u8&`, ...
  + `u8|`, ...
  + `u8^`, ...
  + `u8<<`, ...
  + `u8>>`, ... (unsigned shift, for unsigned ints)
  + `i8>>`, ... (signed shift, for signed ints)
  + `u8<<<`, ...: left rotate
  + `u8>>>`, ...: right rotate
+ Float arithmetic
  + `f32ceil`
  + `f32floor`
  + `f32round`
  + `f32sqrt`, ...
  + `f32sin`, ...
  + `f32cos`, ...
  + `f32tan`, ...
  + `f32asin`, ...
  + `f32acos`, ...
  + `f32atan`, ...
  + `f32atan2`, ...
  + `f32exp`, ...
  + `f32log`, ...
  + `f32erf`, ...
  + `f32sfft`, ... (for vectors)
  + `f32sifft`, ...
+ Complex numbers
  + `c32real`, `c64real`
  + `c32imag`, `c64imag`
  + `c32conj`, ...
+ Iteration (integer types)
  + `n [...] i8.`, ...: for-each in range _0..n-1_


### Strings
+ `>b`: tuple/set/vector/UTF8/symbol to bytes
+ `>u`: tuple/set/vector/bytes/symbol to UTF8 (non-validating)
+ `u?`: is UTF8 string valid
+ `b#`: byte-length of string
+ `c#`: character length of UTF8 string
+ `s i b@`: byte at byte-position
+ `s i c@`: char at char-position
+ `s i n b!`: byte-substring of `n` bytes starting at `i`
+ `s i n c!`: char-substring of `n` chars starting at `i`
+ `s u8 b<>`: byte-split into tuple
+ `s sp c<>`: char-split (where `sp` is a string of any length)
+ `s b b?`: find byte in string (return position or -1)
+ `s c c?`: find char in string (return char-position or -1)
+ `s bs bs?`: find bytes in string (return position or -1)
+ `s cs cs?`: find string in string (return char-position or -1)
+ `s1 s2 b++`: append byte-strings
+ `s1 s2 u++`: append UTF8-strings
+ `(s1, s2, ...) b/++`: join byte-strings
+ `(s1, s2, ...) u/++`: join UTF8-strings


### Symbols
+ `@:`: gensym
+ `>@`: bytes/UTF8 to symbol

**TODO:** UTF8/bytes conversions


### Tuples
+ `... x₃ x₂ x₁ n »t`: make tuple from elements
+ `t «t`: flatten tuple to stack, with length on top
+ `>t`: set/map/vector/bytes/UTF8 to tuple
+ `t t#`: tuple length
+ `i t t@`: get element from tuple
+ `t s n t!`: sub-tuple (start + len)
+ `xs ys t++`: append
+ `xs [...] t.`: for-each
+ `xs [...] t*`: map
+ `xs [...] t%`: filter
+ `([...], [...], ...) t*.`: apply each function to the current stack, entuple results
+ `(t1, t2, ...) t/++`: join tuples

**TODO:** primitive tuple-sort on `[] → b` ordering function (return permutation vector)


### Sets
**NOTE:** set elements are stored in hash-order and set operators both deduplicate accordingly and preserve this ordering.

+ `s∅`: empty set
+ `... x₃ x₂ x₁ n »s`: make set from elements
+ `>s`: tuple/vector/bytes/UTF8 to set
+ `x s s?`: check set member
+ `x s s+`: insert element into set
+ `a b s|`: set union
+ `a b s&`: set intersection
+ `a b s-`: set difference (`a - b`)
+ `s [...] s.`: for-each
+ `s [...] s*`: map
+ `s [...] s%`: filter
+ `s s»`: ensure hash-ordering (required before `>i`)


### Maps
+ `m∅`: empty map
+ `... v₃ v₂ v₁ ... k₃ k₂ k₁ n »m`: make map from elements
+ `>m`: tuple/set/vector/bytes/UTF8 of pairs → map
+ `vs ks ^m`: zip tuples/vectors into map
+ `m mk`: tuple of map keys
+ `m mv`: tuple of map values
+ `k m m@`: map k/v lookup
+ `v k m m+`: insert k/v pair into map
+ `b a m|`: map union, prefer values from `b`
+ `b a m&`: map intersection, prefer values from `a`
+ `m [...] m.`: for-each (`k v → ...`)
+ `m [...] m*`: map (`k v → v'`)
+ `m [...] m%`: filter (`k v → b`)
+ `m m»`: ensure hash-ordering (required before `>i`)


### Indexes
+ `x b >i`: index `x` with `b` bits of approximation; returns indexed structure
  + This operator auto-detects the type of `x`, which can be `tuple`, `set`, or `map`
  + **NOTE:** `set` and `map` are currently unimplemented
+ `i<`: fetch index vector
+ `i>`: fetch indexed object
+ `i#`: number of bits for index


## λ
+ `x [...] λ:`: create λ in same global scope, return `λi`
+ `x [...] λ∷`: create λ in forked global scope, return `λi`
+ `λi λ_`
+ `λy`: yield out
+ `dt Θd`: sleep nanoseconds
+ `t Θt`: sleep until nanosecond timestamp (TODO)


## η
+ `η:`: create η
+ `η η_`: destroy η
+ `v k η η=`: create binding in η -- if `k == ω`, then delete binding
+ `k η η@`: access binding in η
+ `k η η?`: check for binding existence


## φ
+ `ζb fs φ:`: create φ, return `φ*`
+ `φ* φ_`
+ `φa φb φ|`: connect φa to φb, creating ζs as necessary
+ `φa φb φ<>`: splice element from φa to φb (returns bool)
+ `φa φb φ<>=`: non-connect-blocking splice from φa to φb (returns bool)
+ `x φ φ<`: write element to φ (returns bool)
+ `x φ φ<=`: non-connect-blocking write to φ (returns bool)
+ `φ φ*`: peek element from φ (returns `ω` on EOF)
+ `φ φ++`: advance φ
+ `φ φ<?`: is φ writable
+ `φ φ*?`: is φ readable
+ `φ φω?`: is φ closed
+ `φ φω`: close write-end of φ


### Circular iteration
+ `φ [...] τ.`: for each element in cycle (TODO)
+ `φa φb τ|`: splice one cycle from φa to φb (TODO)
+ `φ τ>t`: _τ_-delineated cycle to tuple
+ `φ τ>s`: cycle to set
+ `φ τ>m`: cycle to map (elements are coerced to pairs, defaulting to `t`)
+ `φa φb [...] τ^φ`: ordered zip → φ (TODO)


### Iteration
+ `φ [...] φ.`: for each element


## π interpreter API
You can use π to create a sub-interpreter, effectively allowing you to JIT code. This sub-interpreter can also catch what would normally be fatal exceptions, e.g. for incorrect numeric casts or bounds errors.

**TODO:** define this API


## Linux native API
+ Filesystem stuff
+ FIFOs
+ Sockets
+ FD/φ IO
+ Process API (`fork`/`wait`)
+ X11/GL


## WASM API
+ Websocket/φ
+ DOM calls
+ DOM events → φ
+ Local storage
+ Canvas GL
