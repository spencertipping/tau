# π interpreter
Consumes the output of the [π compiler](pi-compiler.md), which is a C-style imperative structure that we SSA down to garbage-collected register bytecode. I would use LLVM, but it doesn't support JIT in WebAssembly to my knowledge.

π interpreter values are all heap-allocated UTF9s that never refer to one another. This means GC is a flat loop over the register table, making generational GC trivial to implement.

**TODO:** define interpreter structure
