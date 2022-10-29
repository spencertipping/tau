# π₀ GC
[π₀](pi0.md) was created to solve the "transform things without copying" problem: rather than having γs that consume and produce full UTF9 values, we have functions that refer to parts of those values, copying only what's needed.

Taken to its logical conclusion, we end up with a shared heap that can span all γs within a Φ. All that's needed is a way to drop heap-references into UTF9 values, rewriting them as needed at the Φ boundaries. This is all invisible to the user, who can assume UTF9s are immutable and manipulated as though they were being fully copied at every step.

This page is about GC ideas; see [GC implementation](pi0-gc-impl.md) for the algorithm and invariants.


## UTF9 flagging
Most UTF9 values are allocated inline: a tuple physically contains its elements, for example. This is convenient from a GC perspective because it means we can just copy the value as a single block without inspecting its contents, and all internal offsets are also preserved. **However,** that's not the whole story.

If we forced these constraints onto all heap-allocated UTF9 values, we'd effectively wind up copying most of the stuff we want to avoid. Instead, we allow references, defined by the [π₀ subtype of UTF9](pi0-utf9.md), which can be dropped into arbitrary locations in UTF9. Functions that work with UTF9 will dereference these, and the GC will transform them as necessary -- just like pointers are updated in traditional mark/sweep collectors.

Every UTF9 value that contains a reference must be transformed, either to rewrite or to inline that reference. We mark UTF9 values using the [flag](utf9.md#flags) bit, which applies upwards. References are flagged, which automatically causes anything that contains them to be flagged as well.

So: un-flagged values can be moved as opaque data, flagged values have internal fields that must be transformed. `π₀h.simplify(i9) → O9` will remove any references and provide an object that can be written to a boundary [Φ](Phi.md).


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
This is an opt-in way for native code to have `i9` variables that are kept up-to-date by GC -- sort of like having the GC trace the stack, but on a more selective basis.

For example, here's how we implement `m.`, "for each k/v pair in a map":

```cpp
π0hnf f{z.h};
π0bi fn = z.pop();    // the function bytecode
i9   m  = z.pop();    // the map (tracked by GC)
i9   i  = m.first();  // iterator (tracked by GC)
f(m, i);              // enable GC tracking
while (i < m.next())
{
  z.push(i); ++i;
  z.push(i); ++i;
  z.run(fn);  // if this causes GC, m and i are updated
}
// f auto-removes itself from GC scope when destructed
```

Notice that we recompute `m.next()` instead of having a third GC-tracked variable. The reason is that `m.next()` refers to a separate object that may or may not be defined; the GC doesn't understand that we mean "the thing after `m`" rather than whatever `m.next()` refers to.

If we wanted to cache `m.next()` to avoid recomputation, we'd need to set up a GC-variant quantity, which we can do by specifying a function that calculates it after each GC:

```cpp
i9 m = ...;
i9 i = ...;
i9 e;
f(m, i, [&]() { e = m.next(); });
```


### GC locking
We can't tell the GC to magically not run because it may legitimately need to run a compaction to find new space. But we can write code that fails loudly if GC runs where we don't expect it.

```cpp
π0hgl l{z.h};  // crashes if GC runs while in scope
```


## Generational heaps
Mark/sweep requires that we copy live objects from one heap space into another, which means that the GC maintains distinct memory regions, each of which can be replaced by its new copy-into space -- more precisely, we `malloc` the new region and `free` the old one once the copying is done.


### No write barriers
Because UTF9 values are immutable, we will never have oldgen values referring to newgen ones. This means no insta-tenuring or other complications: each oldgen is completely self-contained relative to newer ones.


### Tenuring
UTF9 doesn't provide any room for metadata to store details like "how many generations has this object been alive". I could allocate more [π₀ UTF9](pi0-utf9.md) space for this, but for now we just tenure after a single generation. If we have four heaps, the final generation requires the object to live for roughly eight new-gen collection cycles.


### Resizing
A single-generation GC is easy to resize: we just apply some function to the live set size and use that for the new-space. Generational GC is different; we usually want the newest generation to be fixed-size and older generations to resize adaptively. The oldest generation tends to take up most of the slack.

This is all generalized in π₀ GC by having a heap-resize function that gets invoked whenever any generation is collected.


## Contained references
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
