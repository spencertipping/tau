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

+ `n :&`: create a frame with _n_ locals
+ `_&`
+ `n &@`: get local
+ `x n &=`: set local

`&@` and `&=` are slightly slower than named variable accessors because `n` is pushed onto the stack.


## Control flow
+ `[...] .`: run bytecode function
+ `c [t] [e] ?`: if `c` is true, run `[t]`; else run `[e]`
+ `[c] [b] ?.`: while `[c]` returns true, do `[b]`


## UTF9
### General
+ `?e`: does the value exist?
+ `?t`: what is the value type? (returns symbol)


### Vectors
+ `xs t >v`: tuple/set/bytes/UTF8 to typed vector
+ `xs vlen`: vector length
+ `xs vtype`: symbol (`i8`, `i16`, ..., `u64`, `f32`, ..., `c64`)
+ `xs i vget`: get vector element
+ `... x₃ x₂ x₁ n t vec`: make vector of type `t` (a symbol, see `vtype`)


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
+ Integer arithmetic
  + `i8+`: undefined on overflow
  + `i16+`
  + ...
  + `u64+`: wrap on overflow/underflow
  + `i8-`, ...
  + `i8*`, ...
  + `i8**`, ...: discrete exponent
  + `i8/`, ...
  + `i8%`, ...
  + `i8n`, ...: negate
+ Float arithmetic
  + **TODO**
+ Bitwise
  + `u8&`, ...
  + `u8|`, ...
  + `u8^`, ...
  + `u8<<`, ...
  + `u8>>`, ... (unsigned shift, for unsigned ints)
  + `i8>>`, ... (signed shift, for signed ints)
  + `u8<<<`, ...: left rotate
  + `u8>>>`, ...: right rotate


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
+ `>t`: set/map/vector/bytes/UTF8 to tuple
+ `t t#`: tuple length
+ `t i t@`: get element from tuple
+ `t s n t!`: sub-tuple (start + len)
+ `... x₃ x₂ x₁ n tuple`: make tuple from elements
+ `xs ys t+`: append
+ `xs [...] t.`: for-each
+ `xs [...] t*`: map
+ `xs i [...] t/`: reduce


### Sets
+ `>s`: tuple/vector/bytes/UTF8 to set
+ `s x s?`: check set member
+ `a b s|`: set union
+ `a b s&`: set intersection
+ `a b s-`: set difference (`a - b`)
+ `xs [...] s.`: for-each
+ `xs [...] s*`: map
+ `xs i [...] s/`: reduce


### Maps
+ `>m`: tuple/set/vector/bytes/UTF8 of pairs → map
+ `ks vs ^m`: zip into map
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
+ `m i [...] m/`: reduce
+ `m i [...] mv/`: reduce values


### Indexes
**TODO**


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
+ `fi [...] φ.`: for-each element
+ `fi i [...] φ/`: reduce
