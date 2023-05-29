# π interpreter
A π program state consists of:

1. A heap
2. A stack of immediate heap refs
3. A map of bound global variables
4. The current `ξi`
5. The current `ξo`


## Register vs heap values
The heap is a linear buffer of non-prefixed η values; for example, a heap consisting of two 8-bit ints `3` and `4` would have these bytes:

```
11 03 11 04
```

Registers, i.e. heap pointers, encode the beginning and length of the value they refer to. In this case, the two register values for this heap would be `0+2` and `2+2`.


## Stack values vs η plurality
Each register/stack value can refer to a η buffer, and each η buffer can hold multiple values. A function can destructure the stack: `(args...) → T<result...>`, or it can coerce its arguments as singles: `(i64, f64, St) -> i64`, or it can accept its arguments as tuples to destructure: `(T<i64, f64>, T<St, St>) -> T<f64, i64>`.

Bare arguments bound as `ηi` or `Sn<u8c>` will match the η value directly. You can extract a single embedded element using `T<ηi>` or `T<Sn<u8c>>`.


## GC
GC has the following properties:

1. All values are flat; they cannot refer to one another
2. The root set consists entirely of off-heap registers that define origin + extent
3. Values are typically buffered before being committed to the heap, deferring GC
4. Nesting is preserved; if `ref1` is inside `ref2`, then this is also true post-GC (not semantically important; we just do this for performance, to avoid duplicating inner values)
5. Because source values may refer to the heap, we use a semispace algorithm to collect-on-allocation and we preserve the old heap until all values are written
