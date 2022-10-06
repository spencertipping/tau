# π₀ standard library
The standard library ships in versions that you can enable per program, so I don't have to be especially careful about backwards compatibility.


## UTF9
### General
+ `?e`: does the value exist?
+ `?t`: what is the value type? (returns symbol)


### Vectors
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
+ Arithmetic
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
+ `>set`: tuple/vector/bytes/UTF8 to set
+ `s x s?`: check set member
+ `a b s|`: set union
+ `a b s&`: set intersection
+ `a b s-`: set difference (`a - b`)


### Maps
+ `m k mget`: get key from map


### Indexes
**TODO**


## γ/φ
+ `x fi φ<<`


## Stack manipulation
Anything complicated will use frames, but it's good to have a few functions to minimize the GC impact of having frame-scoped data.

+ `dup`
+ `duup`
+ `dip`
+ `diip`
+ `drop`
+ `swap`


### Frames
Mostly enabled with syntax, but we should also have explicit instructions:

+ `n fpush`: create a frame with _n_ locals
+ `fpop`
+ `n fget`
+ `x n fset`

`fget` and `fset` are slightly slower than named variable accessors, as each involves an extra stack operation.
