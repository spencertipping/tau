# π₀ bytecode
Exactly like [Joy](https://en.wikipedia.org/wiki/Joy_(programming_language)): we have a very cheap evaluation function that simply proceeds through a list, executing each element in turn. Local scopes are available by pushing/popping them. You can refer upwards to parent scopes if you want to.

There are three stacks: data, return, and frame. Each frame reserves a preallocated local variable space against which you can issue get/set commands to minimize stack manipulation. Function arguments and return values are passed on the stack.


## Program format
Joy models literals and quotations as functions-that-push-values, which I think is correct for π₀ as well. I was initially hesitant because it requires that we customize the bytecode, but let's suppose our bytecode is just function pointers to start with. Then it's simple, and bytecode isn't expected to be portable in any case. Every program is run through the assembler to produce transient bytecode; that's useful because it means the _assembler_ is the format we define, not the bytecode itself.

**NOTE:** we will probably want functions to be some data structure like they are in Joy, so it's worth thinking that through. Maybe the `[]` can begin with its own quotation program.
