# π₀ standard library
The standard library ships in versions that you can enable per program, so I don't have to be especially careful about backwards compatibility.


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

+ `n &:`: create a frame with _n_ locals
+ `&_`
+ `n &@`: get local
+ `x n &=`: set local

`&@` and `&=` are slightly slower than named variable accessors because `n` is pushed onto the stack.


## Control flow
+ `[...] .`: run bytecode function
+ `c [t] [e] ?.`: if `c` is true, run `[t]`; else run `[e]`
+ `[b] [c] ?!`: while `[c]` returns true, do `[b]`
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
+ `$b`: UTF9 → `bytes` (flatten + serialize)
+ `b$`: `bytes` → UTF9 -- deserialize and validate (invalid → `none`)
+ `b$!!`: `bytes` → UTF9 -- deserialize and don't validate (dangerous)


### Structure literals
+ `(`: begin tuple (fork stack)
+ `)`: end tuple (pop forked stack)
+ `{`: begin map
+ `}`: end map
+ `,`: emit tuple element


### Vectors
In the functions below, `v` is replaced by a primitive type, e.g. `i8s`. So `i8s#` returns the number of elements in a vector of `i8`s.

+ `xs t >v`: tuple/set/bytes/UTF8 to typed vector
+ `... x₃ x₂ x₁ n t >>v`: make vector of type `t` (a symbol, see `?t`)
+ `xs v#`: vector length
+ `xs i v@`: get vector element
+ `xs i n v!`: sub-vector
+ `a b v++`: concatenate vectors
+ `xs [...] v.`: vector for-each
+ `xs [...] v*`: vector elementwise map
+ Native reductions
  + `xs v/+`: sum of elements
  + `xs v/*`: product of elements
  + `xs v/<`: check for descending order
  + `xs v/&`: bitwise-and of elements
  + `xs v/|`: bitwise-or of elements
  + `xs v/^`: bitwise-xor of elements


### Numbers
All of these operators automatically distribute across vectors.

+ Type conversions
  + `>i8`: saturate on overflow
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
+ Integer arithmetic
  + `i8l`, ...: nonzero → 0, 0 → 1 (`l` instead of `!`, which means "substring" in π₀)
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
  + `[...] n i8.`, ...: for-each in range _0..n-1_


### Strings
+ `>b`: tuple/set/vector/UTF8 to bytes
+ `>u`: tuple/set/vector/bytes to UTF8
+ `u?`: is UTF8 string valid
+ `b#`: byte-length of string
+ `c#`: character length of UTF8 string
+ `s i b@`: byte at byte-position
+ `s i c@`: char at char-position
+ `s i n b!`: byte-substring of `n` bytes starting at `i`
+ `s i n c!`: char-substring of `n` chars starting at `i`
+ `s u8 b<>`: byte-split into tuple
+ `s sp c<>`: char-split (where `sp` is a string of any length)


### Tuples
+ `... x₃ x₂ x₁ n >>t`: make tuple from elements
+ `t <<t`: flatten tuple to stack, with length on top
+ `>t`: set/map/vector/bytes/UTF8 to tuple
+ `t t#`: tuple length
+ `t i t@`: get element from tuple
+ `t s n t!`: sub-tuple (start + len)
+ `xs ys t++`: append
+ `xs [...] t.`: for-each
+ `xs [...] t*`: map
+ `xs [...] t%`: filter
+ `([...], [...], ...) t*.`: apply each function to the current stack, entuple results


### Sets
**NOTE:** set elements are stored in hash-order and set operators both deduplicate accordingly, and preserve this ordering.

+ `... x₃ x₂ x₁ n set`: make set from elements
+ `>s`: tuple/vector/bytes/UTF8 to set
+ `s x s?`: check set member
+ `a b s|`: set union
+ `a b s&`: set intersection
+ `a b s-`: set difference (`a - b`)
+ `xs [...] s.`: for-each
+ `xs [...] s*`: map
+ `xs [...] s%`: filter


### Maps
+ `... v₃ v₂ v₁ ... k₃ k₂ k₁ n map`: make map from elements
+ `>m`: tuple/set/vector/bytes/UTF8 of pairs → map
+ `ks vs ^m`: zip tuples/vectors into map
+ `m mk`: tuple of map keys
+ `m mv`: tuple of map values
+ `m k m@`: map k/v lookup
+ `m ks m<`: multiple map k/v lookup
+ `a b m|`: map union, prefer values from `b`
+ `a b m&`: map intersection, prefer values from `a`
+ `m [...] m.`: for-each
+ `m [...] mk.`: for-each key
+ `m [...] mv.`: for-each value
+ `m [...] m*`: map
+ `m [...] mv*`: map values
+ `m [...] m%`: filter
+ `m [...] mv%`: filter values


### Indexes
+ `x f >i`: index `x` with coverage factor `f`; returns indexed structure
  + This operator auto-detects the type of `x`


## γ
### λ
+ `[...] λc`: create λ, return `λi`
+ `li λx`
+ `λy`: yield out
+ `dt Θd`
+ `t Θt`


### Low-level φ
+ `x fi >φ`: write element to φ (returns bool)
+ `x fi >=φ`: non-connect-blocking write to φ (returns bool)
+ `fi <φ`: read element from φ (returns `ω` on EOF)
+ `fi φ>?`: is φ writable
+ `fi φ<?`: is φ readable
+ `fi φω?`: is φ closed


### Circular iteration
+ `τ>t`: _τ_-delineated cycle to tuple
+ `τ>s`: to set
+ `τ>m`: to map (elements are coerced to pairs, defaulting to `t`)


### Iteration
+ `fi [...] φ.`: for each element


## π₀ interpreter API
You can use π₀ to create a sub-interpreter, effectively allowing you to JIT code. This sub-interpreter can also catch what would normally be fatal exceptions, e.g. for incorrect numeric casts or bounds errors.

**TODO:** define this API
