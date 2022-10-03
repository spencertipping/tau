# π₀ GC
π₀ programs are stack-oriented bytecode, so we need to track their values accordingly. We do this with the usual three-tier setup:

1. A stack for immediate expressions
2. A stack of frames for local variables
3. A heap for indefinite lifetimes

Unlike most languages, pointers are uncommon and we're free to move heap-allocated values. UTF9 is semantically immutable, so we don't have to follow any particular aliasing strategy. π₀'s GC prefers to be a cheap copying mark/sweep collector, with some exceptions for more complex cases.

All values are encoded as UTF9 when stored, which sounds like a problem because UTF9 doesn't support pointers-to-things; each value is fully serialized inline. Luckily the [UTF9 spec](utf9.md) reserves a block of values with bit-prefix `11100fss` -- `fss` encodes the flag and size -- that π₀ is free to use in any way whatsoever. Some of these values are pointers, the only UTF9 values that set the flag (which applies transitively upwards); see [π₀ UTF9](pi0-utf9.md).


## "Simple" vs "complex" values
UTF9 is usually very cheap to GC because it needs to be neither scanned nor transformed when it is moved. This is the common "simple value" case. The exception is when the value contains a pointer to part of another value; then the value is "complex" and requires the usual trace-and-rewrite.

Any value stored in a π₀-managed heap uses its flag field to indicate complexity. Or to say the same thing differently, all π₀ UTF9 internal values set the flag, and no other type of value is allowed to do this within the scope of a π₀ runtime.


## GC-invariant native FFI
Any GC that moves heap values needs to rewrite pointers to those values. Because `i9`s are reference types, they are themselves pointers -- which means they must be rewritten. C++ doesn't provide managed stacks, so any local variables must be GC-invariant. That means we need a pointer-lookup table, in this case called the _pin set._

The pin set is small by design: it contains just the set of variables in use for a single native function call. We reset the pin set whenever the native C++ function is done executing; at this point we know there are no external references.
