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
UTF9 values are very much by-reference, even simple ones like integers. For obvious reasons, the GC shouldn't move a value that's actively being used by a C++ function; so we have two mechanisms that allow C++ to coexist with a GC that will move values arbitrarily.

1. Short-term heap pinning
2. Longer-term native frames

Heap pinning is intended for very short-term use: for example, vectorized addition in which the inputs will be streamed into the output. Space for the output is allocated before the inputs have been consumed, so the inputs may be moved into a new heap after we've read their sizes and before we've streamed the addition.

To fix this, we tell the GC to pin the old heap until we've done the addition, then release it when we say. This is heavy-handed because the old heap is potentially quite large, but the idea is that the operation needs to be low-latency and completes in a very small timeframe.

Some operations require longer-term access to UTF9 values. These use native frames, which are just like regular frames but aren't visible to π₀ bytecode -- that is, they're on a separate stack. This allows old heaps to be deallocated but preserves specific values across GC generations.


### Example: π₀ `m.`
`m [...] m.` iterates over all `k v` pairs in `m`, invoking `[...]` for each one. `[...]` is allowed to allocate memory and may be arbitrarily complex, so we use a native frame to contain `m`. But there's a new problem: what do we do with the iterator? We could store it relative to `m`'s base address, but even that won't work because `m` could be complex, which means that the iterator position will be modified arbitrarily by GC.

We solve this by tracking both `m` and `i` (the iterator) in the native frame. GC will update both in a λ-atomic way. This means we must alias C++ locals to GC-owned values, which in turn means these natives should be stored as real memory addresses, not heap-offsets. (This way the C++ function can write to them directly.)

```cpp
auto f  = π0nF(z.h, 2);
uN   fn = z.pop();         // the function bytecode
auto &m = f << z.pop();    // the map
auto &i = f << m.first();  // iterator

while (i < m.end())
{
  // loop stuff, including z.run() for each entry
  // if GC happens, both m and i will be updated
}

// f will fall out of scope, removing it from the pin set
```
