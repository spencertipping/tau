# π₀ standard library
## UTF9
**TODO**


## γ/φ
**TODO**


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
