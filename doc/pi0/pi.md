# π
We need a native way to transform UTF9 values that doesn't result in tons of custom C++. That is, we need a little bytecode language with a simple generational GC. π source is UTF9, but with a fair bit of typesetting/layout in the frontend.

π matters because without it, we have to create a separate γ for every degree of composability we want to present to the user. Beyond being inefficient due to excessive value copying, this would also require lots of awkward micro-γs to serve as adapters. π provides a small random-access context to eliminate this overhead.

**TODO:** rewrite most of the pages below; it's good for a first draft, but my thought process has changed enough that there's a lot that can be packed out

+ [π ideas](pi-ideas.md)
  + [π ← `ni`](pi-ni.md)
  + [π editor in π](pi-editor.md)
+ [π design](pi-design.md)
  + [π structure](pi-structure.md)
  + [π history](pi-history.md)
+ [π compiler](pi-compiler.md)
  + [π intent](pi-intent.md)
  + [π parsing](pi-parsing.md)
  + [π assembler](pi-asm.md)
+ [π runtime](pi-runtime.md)
  + [π interpreter](pi-interpreter.md)
  + [π gc](pi-gc.md)
