# π interpreter
Consumes the output of the [π compiler](pi-compiler.md).

Let's imagine how this might work. We have UTF9 values arranged as tuples, some of which contain more tuples. The first entry is the operation being performed (possibly as a native function pointer, or an index into such a table), the rest are its arguments.

UTF9 is a great format for this because it's naturally tail-recursive; the final entry within a tuple need not be aware that it was in a tuple. For example, in the stream `(a, b, c), d`, both `(a, b, c)` and `c` will be followed immediately by `d`; we can work with `c` without any awareness that it was located inside a container. (Note that this is true only when the tuple has no index, which will always be the case for sexp-style trees.)


## Heterogeneous operators
Unstructured data processing often involves situations where I've asserted the type of only one of several fields, say in a tuple. I'll sometimes want to operate on the tuple in some way without full destructuring. Ideally this can be done (1) atomically, and (2) with a homogeneous vector some of whose entries are zero (or semantic equivalent).

This means π needs a robust definition of algebraic identity that acts as a short-circuit passthrough; nothing about the operand is inspected when identity-transforming it.


## Local ξ



## Control flow
