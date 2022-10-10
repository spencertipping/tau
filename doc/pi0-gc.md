# π₀ GC
π₀ was created to solve the "transform things without copying" problem: rather than having γs that consume and produce full UTF9 values, we have functions that refer to parts of those values, copying only what's needed.

Taken to its logical conclusion, we end up with a shared heap that can span all γs within a Φ. All that's needed is a way to drop heap-references into UTF9 values, rewriting them as needed at the Φ boundaries. This is all invisible to the user, who can assume UTF9s are immutable and manipulated as though they were being fully copied at every step.


## Live set
We want to share UTF9 values between λs, both in ζs and in shared variable frames, e.g. a set of globals per γ. Additionally, each λ has its own stack of data and local-variable frames. That means our γs have a live set that looks like this:

```
γ₁ +-- globals
   |
   +-- λ₁ +-- data stack
   |      +-- frame stack --- [frame]
   |
   +-- λ₂ +-- data stack
   |      +-- frame stack --- [frame]
   ...

γ₂ +-- globals
   |
   ...

γ₃
...
```

We also need register aliases for native C++ functions that hold references to heap-owned data for longer than we want to pin the heap. Each of these is an ad-hoc list of `i9&` values that will be updated automatically when GC happens.

Because the live set is open-ended, we'll use a set of virtual pointers that provide "views" into the heap. These implement various abstractions like data stacks, frame stacks, globals, and native frames.


## UTF9 flagging
Most UTF9 values are allocated inline: a tuple physically contains its elements, for example. This is convenient from a GC perspective because it means we can just copy the value as a single block without inspecting its contents, and all internal offsets are also preserved. **However,** that's not the whole story.

If we forced these constraints onto all heap-allocated UTF9 values, we'd effectively wind up copying most of the stuff we want to avoid. Instead, we allow references, defined by the [π₀ subtype of UTF9](pi0-utf9.md), which can be dropped into arbitrary locations in UTF9. Functions that work with UTF9 will dereference these, and the GC will transform them as necessary -- just like pointers are updated in traditional mark/sweep collectors.

Every UTF9 value that contains a reference must be transformed, either to rewrite or to inline that reference. We mark UTF9 values using the [flag](utf9.md#flags) bit, which applies upwards. References are flagged, which automatically causes anything that contains them to be flagged as well.

So: un-flagged values can be moved as opaque data, flagged values have internal fields that must be transformed.


## Heaps, references, and inlining
Every UTF9 value lives in a heap and can be moved. There may be more than one heap; typically they're arranged in a generational structure. References disambiguate heaps by using high-order bits.

Because UTF9 values are immutable, duplication is not a problem; this means any value smaller than some size can be inlined directly into wherever the reference would go. This, in turn, means that _any reference target can be guaranteed to be at least a certain size,_ which is convenient because it gives us a place to store new-heap destination addresses as the GC is running:

```
cb [sb] |.....|    ← initial value
        |<--->|    ← at least the inlining size

π₀cb [sb] | newheap_addr |  ← rewritten value
          |<------------>|  ← ≤ inlining size
```

**TODO:** the above is cool but also a lie: two pointers can be too close together. See contained-regions below. We'll probably end up dropping this.


### Contained-region detection
Consider this scenario:

```
             i₁ (unflagged)
             |
             V
tuple [sb] bytes [sb] "foobar" i64 [sb] 12452135
  ^
  |
  i₀ (unflagged)
```

This is perfectly legal; neither the outer tuple nor the inner `bytes` is flagged, yet both references must be rewritten. If the address mapping is ordered, we can use next-lowest address as the key lookup. This increases GC to _O(n log n)_ complexity in the live set size, but that should be fine; _log n_ is likely smaller than the allocation per element.


### Spliced rewriting
How do we rewrite flagged values efficiently? For example, the least-efficient strategy would be to build an `o9V` for each container and rebuild everything at a low level. The best case is to break each flagged UTF9 into as few contiguous regions as possible and `memcpy` each, then patch in the small changes.


# _
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


### Example: π₀ `λc`
`[...] λc` requires us to fork the interpreter: `[...]` is written with the same bytecode as the rest of the program, but will execute with a separate heap and stacks.

A practical issue is that λs need a way to communicate with one another, but π₀ doesn't provide one; each λ runs with a separate interpreter and heap. This limitation defeats much of the purpose of π₀ in the first place, so let's change it:

**FIXME:** π₀ must support multiple λs running against the same heap, so they can share memory without copying

**NOTE:** we should compartmentalize stack-sets like we do native frames: these should be views onto the heap, and although the heap knows how to update them, it doesn't own them. This representation also simplifies generational GC by quite a lot.

**NOTE:** this also opens up the possibility that multiple γs within the same Φ can share a heap, which in turn means that we don't have to copy values to transfer between γs. That should be a big win for performance; now the copy step is pushed all the way out to the Φ IO boundary if we want it to b
