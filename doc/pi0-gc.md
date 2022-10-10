# π₀ GC
π₀ was created to solve the "transform things without copying" problem: rather than having γs that consume and produce full UTF9 values, we have functions that refer to parts of those values, copying only what's needed.

Taken to its logical conclusion, we end up with a shared heap that can span all γs within a Φ. All that's needed is a way to drop heap-references into UTF9 values, rewriting them as needed at the Φ boundaries. This is all invisible to the user, who can assume UTF9s are immutable and manipulated as though they were being fully copied at every step.


## UTF9 flagging
Most UTF9 values are allocated inline: a tuple physically contains its elements, for example. This is convenient from a GC perspective because it means we can just copy the value as a single block without inspecting its contents, and all internal offsets are also preserved. **However,** that's not the whole story.

If we forced these constraints onto all heap-allocated UTF9 values, we'd effectively wind up copying most of the stuff we want to avoid. Instead, we allow references, defined by the [π₀ subtype of UTF9](pi0-utf9.md), which can be dropped into arbitrary locations in UTF9. Functions that work with UTF9 will dereference these, and the GC will transform them as necessary -- just like pointers are updated in traditional mark/sweep collectors.

Every UTF9 value that contains a reference must be transformed, either to rewrite or to inline that reference. We mark UTF9 values using the [flag](utf9.md#flags) bit, which applies upwards. References are flagged, which automatically causes anything that contains them to be flagged as well.

So: un-flagged values can be moved as opaque data, flagged values have internal fields that must be transformed. `π₀h.simplify(i9) → O9` will remove any references and provide an object that can be written to a boundary Φ.


## GC structure
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

We also need register aliases for native C++ functions that hold references to heap-owned data. Each of these is an ad-hoc list of `i9&` values that will be updated automatically when GC happens. See ["native frame"](#native-frame) below for details.

Because the live set is open-ended, we'll use a set of virtual pointers that provide "views" into the heap. These implement various abstractions like data stacks, frame stacks, globals, and native frames.


### Data stack
The simplest abstraction: it's just a series of `π₀r` values in a vector, the back of which is the top of the stack.


### Frame stack
Internally, two stacks: one of `π₀r` values (the data), and one of `uN`s describing the size of each frame. We resize the data stack by a frame size at a time, but the GC process treats all of the frame data uniformly.

Because frame registers may not have been initialized, we use `π₀hω` as a null reference that is stable under GC and cannot be used by π₀ bytecode.


### Native frame
This is an opt-in way for native code to have `i9` variables that are kept up-to-date by GC -- sort of like having the GC trace the stack, but on a much more selective basis.

For example, here's how we implement `m.`, "for each k/v pair in a map":

```cpp
auto f  = π0nF(z.h, 2);
π0b  fn = z.pop();         // the function bytecode
auto &m = f << z.pop();    // the map (tracked by GC)
auto &i = f << m.first();  // iterator (tracked by GC)
while (i < m.end())
{
  z.push(i); i = i.next();
  z.push(i); i = i.next();
  z.run(fn);  // if this causes GC, m and i are updated
}
// f auto-removes itself from GC scope when destructed
```


### λ/GC atomicity
Every λ runs until it explicitly yields, either by calling `λy` or by blocking on ζ IO. That means we don't have the usual concerns about GC running during a loop unless that loop allocates memory.

This is useful because it means we can write loops that assume nothing will move, e.g. for vectorized math:

```cpp
auto f  = π0nF(z.h, 2);
auto &a = f << z.pop();      // operand
auto &b = f << z.pop();      // operand
let  o  = vectorized(a, b);  // some vector container
z.push(o);                   // the only place GC can happen
i64 *as = a.first();         // no GC from here down
i64 *bs = b.first();
i64 *cs = z.peek().first();
for (uN i = 0; i < a.vn(); ++i) c[i] = a[i] + b[i];
```

So we get full native performance with no write barriers or other thread-aware constructs, since π₀ is ultimately single-threaded.


## Generational heaps
Mark/sweep requires that we copy live objects from one heap space into another, which means that the GC maintains distinct memory regions, each of which can be replaced by its new copy-into space -- more precisely, we `malloc` the new region and `free` the old one once the copying is done.

All of this means that the GC's pointer space will be discontinuous, which requires us to have a fast way to map references to heap regions. We do this by commandeering the high bits of each reference; if we want four generations, we'd use two bits:

```
// π₀r = uN, which is u32 in this example
ref = hh aaaaaa aaaaaaaa aaaaaaaa aaaaaaaa
      |  ----------------+----------------
      |                  |
      heap identifier    address within heap
```

This doesn't give us invariance across GC because we still compact the new-set. It just gives us a fast way to calculate heap-relative addresses and allows us to independently compact old generations.


### Tenuring
UTF9 doesn't provide any room for metadata to store details like "how many generations has this object been alive". I could allocate more [π₀ UTF9](pi0-utf9.md) space for this, but for now we just tenure after a single generation. If we have four heaps, the final generation requires the object to live for roughly eight new-gen collection cycles.


## Nested references
Most heaps use pointers for all references, which reduces the GC problem down to pointer rewriting -- they don't have to care about the relative positions of objects in memory. UTF9 is different because its default mode of operation is that containers like tuples physically contain their elements. This complicates the picture in two ways: two references may refer to the same copy-region, and containers may change size as we dereference internal elements.


### Inlining
Small objects tend to be written directly into their containers even if this causes duplication. The trade can be worthwhile because we reduce the number of flagged objects that require full or partial tracing.

References can sometimes be collapsed into direct children of the containers they belong to. Any singly-referenced object is automatically inlined, and multiply-referenced objects can be inlined into any one of their reference sites.

**NOTE:** there's a distinction between heap→heap and stack→heap references: a heap object with multiple stack references may be "singly referenced" or "unreferenced" because no other _heap_ object refers to it; that is, nothing competes for its locality.


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

There are a few ways we can handle this:

1. Mark _i₀_ and _i₁_ as separate regions, duplicating _i₁_ in the new heap
2. Mark _i₀_ and _i₁_ separately, but have _i₀ → i₁_ as a complex ref to avoid duplication
3. Mark _i₀_ only, moving _i₁_ as a child pointer

Of these, (3) is the optimal strategy: it avoids duplication and allows _i₀_ to remain unflagged because it won't contain any new references.

This is a relatively subtle aspect of design, so I'm splitting it into [its own document](pi0-gc-contained-regions.md) for full analysis.


### Spliced rewriting
How do we rewrite flagged values efficiently? For example, the least efficient strategy would be to build an `o9V` for each container and rebuild everything at a low level, incurring lots of virtual-method overhead in the process. The best case is to break each flagged UTF9 into as few contiguous regions as possible and `memcpy` each, then patch in the small changes.

All of this to say that rewriting flagged UTF9 values is a complex problem that gets its own `o9` implementation with nontrivial engineering. See [π₀ GC splicing](pi0-gc-splicing.md) for details.


## Latency
π₀'s GC stops the world and runs in a single thread. This can be slow for large heaps, although we use some mitigations to minimize the amount of tracing required. Because GC can be disruptive, we provide an API that allows π₀ systems to negotiate with the GC to manage latency and/or trade space for latency on a temporary basis.


### Runtime tuning
+ Pause time → cost function

**TODO:** explore this


### Incremental GC
My initial impression is that this is complicated and likely not worth it, but let's get into the details. The idea is that we'd begin to trace stacks and frames, marking objects as we go and solving for containment relationships. The GC would yield out, resuming π₀ λs, which would modify the stacks and frames as they run, probably allocating new memory as well.

This is inconvenient because there's no clear relationship between λ throughput and memory allocation (from the GC's perspective); do we know that we're serving the use case by interrupting GC so that λs can progress? Furthermore, these λs allocate freely into the new heap while GC is paused. How much space can they use before we pause, copy over old-heap, and resume that way?

Incremental GC seems time-inefficient and I'm not yet convinced it's worthwhile. We can probably do better by tuning the GC, inlining references to avoid trace/rewrite effort, and making use of generations. π₀ isn't Java, so we aren't likely to have lots of pointers to small, cache-nonlocal objects lying around.

Worst-case we can think about multithreaded GC, which I suspect is preferable to incremental.
