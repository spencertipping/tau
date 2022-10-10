# UTF9 `none`
This is a value that doesn't exist because the process that created it was invalid. For example, a key lookup from a map that doesn't contain that key. `none` tends to propagate through operators like `NaN` in floating-point math.

`none` can also be used to represent an undefined limit; for example, `max()` of an empty list.

`none` can carry data. If present, its first byte describes the type of problem that produced the value; some problem types are followed by further data.


## Error codes
The first byte of data specifies the type of `none` -- i.e. the reason it was created. These are defined fully in `enum class u9_none` in [utf9-types.hh](../tau/utf9-types.hh).

+ `00`: generic error
+ `01`: missing key in map
+ `02`: tuple index out of bounds
