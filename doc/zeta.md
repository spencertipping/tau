# ζ
There are two conceptual pieces: ζ, the queue, and ζb, the backing store for it.


## ζb
ζb is defined with a few invariants:

1. Byte offsets appear to strictly increase, modulo some unsigned number
2. Reference counts apply to 1KiB pages
3. Allocation is allowed only when the total live window is within the capacity
4. Every allocation unit is contiguous

(4) is a pretty big constraint because it interacts with the internal buffering structure. If, for example, we have a ζb whose nominal capacity is 65536B and a 1B object pinned at position 32768, then the largest object we can write is now 32768B -- despite having more nominal capacity available.

We can work around this by reserving 200% of nominal capacity; that guarantees we'll have the full capacity available no matter where an object is pinned. Alternatively, we can just heap-allocate overflowed values and store tags to them. Then capacity serves its intended purpose, providing elasticity, rather than limiting the size of objects that can be enqueued.


### Allocation algorithm
Allocation works a lot like a circular buffer, except that we sometimes decide to wrap early because the next object won't fit between the current write offset and end-of-buffer. So we have three variables:

+ `ri`: read-index of pinned data (earliest live offset)
+ `wi`: next byte to be written (one byte beyond live region)
+ `ci`: clip index, to calculate moduli (the effective wrap length)


### Cyclic buffer states
```
normal state:
       ri                  wi          c
  -----|===================|-----------|
                           |---nc------|  <- space for next object

wrapped state:
       wi            ri           ci   c
  =====|-------------|============|----|
       |----nc-------|                    <- space for next object
                                  |xxxx|  <- lost space (this cycle)
```

In the wrapped state, `wi = ri + ci`, not `ri + c`. As soon as `ri` also wraps around, we reset `ci = c` so the buffer is no longer truncated.


### External addressing and overflow
External addresses (`x`) are large enough to store any number between `0` and `c-1` inclusive, but may easily overflow; for example, `ri`'s logical value will increase indefinitely.

The obvious strategy is to keep a "basis offset" that increases over time, such that `x + bi >= ri && x + bi < wi` in the normal state. However, this means `bi` will need to change -- and when it does, it can't modify the meaning of any existing `x`s that have been issued. Equivalently, `Δbi % c == 0` -- implying `Δbi == c`. This works because we will never hold two references more than `c` bytes apart.

So now let's revisit our model and make _everything_ overflow on the same modular basis. Now `ri <= wi` is not an invariant; `ri > wi` means we're in wrapped state. `x % ci` is the effective buffer position if wrapped, otherwise the position is used as-is.


## References
I want `sizeof(R)` and `sizeof(const_iterator)` to both be `sizeof(void*)`, which we can do in two ways:

1. Point to the byte buffer directly (bad idea)
2. Pack some bits to encode a relative offset

(2) is preferable because it lets ζ move the byte buffer, which is sometimes required for contiguous object access. If we do this, we need to encode a few things:

1. Which ζ contains the data
2. What `T` is (for safety)
3. The offset (within the ζ) of the object being referred to


### ζ indexes
A `uN` covers the full address space available to a process, which means something interesting: we can split the bits between ζ and ζ-local addresses in any way we care to without losing any memory, _provided log₂|ζ| ∈ ℤ_. If |ζ| is not a power of two, then we lose the remainder.

The only thing we have to do is accommodate varying sizes of ζ, which we can do by using the first few bits of the packed reference as a size designator. For example, let's suppose 2¹⁰ ≤ |ζ| ≤ 2²⁶. If we bin those into four size groups, |ζ| ≤ 2¹⁴, |ζ| ≤ 2¹⁸, |ζ| ≤ 2²², and |ζ| ≤ 2²⁶, then we have the following layout for 32-bit machines:

```
11 ZZZZ AA AAAAAAAA AAAAAAAA AAAAAAAA  <- 26 address bits
10 ZZZZZZZZ AAAA AA AAAAAAAA AAAAAAAA  <- 22 address bits
01 ZZZZZZZZ ZZZZ AA AAAAAAAA AAAAAAAA  <- 18 address bits
00 ZZZZZZZZ ZZZZZZZZ  AAAAAA AAAAAAAA  <- 14 address bits
```

That's not bad: we can create 65536 14-bit ζs, 4096 18-bit ones, 256 22-bit ones, and 16 26-bit ones, ultimately jointly covering all of the 4GiB address space if we have exactly that allocation of ζs.

In order to get full address-space efficiency, we need to make sure that we don't have too many spare entries in any of the size bins. We can do this in a couple of ways:

1. Just have a few size bins, like above
2. Try to do some auto-bin-sizing stuff

(1) makes a lot more sense, so I'm going to run with that. (2) isn't easy at all because we can't change the bit allocation once we commit to a bin size, and it would result in a huge mess.


## GC
ζs don't have GC proper, but they do collect memory as you free/destroy `R`s that are no longer in use. This happens by moving the "from pointer" forwards, decreasing the live-memory window.

It would be prohibitive to manage memory at the byte-level, so we manage pages instead. Each page (4KB) has a reference count that `R`s contribute to. Free pages are available for writing; pages with references are write-locked.
