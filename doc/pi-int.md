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


## GC
GC has the following properties:

1. All values are flat; they cannot refer to one another
2. The root set consists entirely of off-heap registers that define origin + extent
3. Values are typically buffered before being committed to the heap, deferring GC

**TODO:** we need to define semispace locking semantics so we guarantee that source values live long enough to copy into destination values (where we have such dependencies)

**TODO:** strongly specify how nested values are handled: we probably want to preserve containment because the root set is unlikely to be so large that this is prohibitive (i.e. we can sort it)
