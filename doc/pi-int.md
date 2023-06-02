# π interpreter
A π program state consists of:

1. A heap
2. A stack of immediate heap refs
3. A map of bound global variables
4. The current `ξi`
5. The current `ξo`


## Register vs heap values
The heap is a linear buffer of η values with relocation prefixes that are used during GC; for example, a heap consisting of two 8-bit ints `3` and `4` would have these bytes:

```
|-----------------|       |-----------------|          <- relocation prefixes
                    |---|                     |---|    <- values
0000 0000 0000 0000 11 03 0000 0000 0000 0000 11 04    <- heap data
     4         8          10        14        18       <- byte offsets
```

Registers, i.e. heap pointers, encode the beginning and length of the value they refer to. In this case, the two register values for this heap would be `8+2` and `18+2`.


## Stack values vs η plurality
Each register/stack value can refer to a η buffer, and each η buffer can hold multiple values. A function can destructure the stack: `(args...) → T<result...>`, or it can coerce its arguments as singles: `(i64, f64, St) -> i64`, or it can accept its arguments as tuples to destructure: `(T<i64, f64>, T<St, St>) -> T<f64, i64>`.

Bare arguments bound as `ηi` or `Sn<u8c>` will match the η value directly. You can extract a single embedded element using `T<ηi>` or `T<Sn<u8c>>`.


## GC
GC has the following properties:

1. All values are flat; they cannot refer to one another
2. The root set consists entirely of off-heap registers that define origin + extent
3. Values are typically buffered before being committed to the heap, deferring GC
4. GC stops the world and updates all refs everywhere
