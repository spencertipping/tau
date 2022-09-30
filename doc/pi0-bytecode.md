# π₀ bytecode
Exactly like [Joy](https://en.wikipedia.org/wiki/Joy_(programming_language)): we have a very cheap evaluation function that simply proceeds through a list, executing each element in turn. Local scopes are available by pushing/popping them. You can refer upwards to parent scopes if you want to.

There are three stacks: data, return, and frame. Each frame reserves a preallocated local variable space against which you can issue get/set commands to minimize stack manipulation. Function arguments and return values are passed on the stack.


## Program format
Joy models literals and quotations as functions-that-push-values, which I think is correct for π₀ as well. I was initially hesitant because it requires that we customize the bytecode, but let's suppose our bytecode is just function pointers to start with. Then it's simple, and bytecode isn't expected to be portable in any case. Every program is run through the assembler to produce transient bytecode; that's useful because it means the _assembler_ is the format we define, not the bytecode itself.

Unlike Joy, π₀ doesn't use runtime lists as an executable format. It does use lists, but data-lists are separate from program-lists. Program-lists are produced ahead of time by the assembler, and take a simple form:

```
f₁ f₂ ... rfn
```

Here, `rfn` is just another function that manipulates the return stack to execute a return, effectively ending the list. It's exactly like `c3` in AMD64.

Each function entry is an offset into a function lookup table constructed by the assembler. The interpreter is specialized to that specific program and is destroyed when the program exits.

This has some important implications. First, it puts a barrier between existing and new code; if a program wants to use the assembler to link a new function, that function must be called with a new interpreter. So JIT contexts are explicit, just as they would be in the real world.

Second, there is no heap or stack sharing; the "run in sub-interpreter" API GC-copies arguments and return values between the two interpreter contexts. We can't have the sub-interpreter borrow the primary interpreter's stack and heap unless we run it serially, which we don't want to have to do.

Although program-specialized bytecode sounds like a liability due to this complexity, it allows us to (1) emit size-optimized bytecode, (2) perform AOT bytecode op-fusion, and (3) avoid any generalized quoting forms, e.g. to push numbers. Linking subprograms isn't very common, so the complexity is worth it to resolve these issues.
