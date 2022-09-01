# ζ
There are two conceptual pieces: ζ, the queue, and ζb, the backing store for it. ζ is higher-level and manages λ scheduling, whereas ζb is really just a bounded linear allocator.


## ζ/λ
ζ can track zero or more λs waiting for either end of the queue to become available. Not much to it, aside from noting that some ζ operations need to be atomic (like read-and-free), but this happens naturally.


## ζb
ζb is defined with a few invariants:

1. Allocation is allowed only when the total live window is within the capacity
2. Every allocation unit is contiguous
3. Values are "read" and then "freed", two distinct phases


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
